#include <ClientDatabase.h>
#include <optional>

#include "LocalUser.h"
#include "LocalServer.h"
#include "Client.h"

ClientDatabase::ClientDatabase() : logger("ClientDatabase")
{}

ClientDatabase::~ClientDatabase()
{}

auto ClientDatabase::AddClient(std::unique_ptr<IClient> new_client) -> IClient*
{
    auto ret = clients_.insert(std::make_pair(new_client->GetUUID(), std::move(new_client)));

    /* Check if duplicate was found */
    if (ret.second == false)
        throw DuplicateClient(); // Should never happen
    return ret.first->second.get();
}

auto ClientDatabase::DisconnectClient(IRC::UUID uuid,
	std::optional<std::string> quit_message) -> void
{
    auto client = GetClient(uuid);
    if (!client)
    {
        logger.Log(LogLevel::ERROR, "No client to delete"); // Should never happen
        return;
    }
    if ((*client)->GetType() == IClient::Type::kUnRegistered)
    {
        logger.Log(LogLevel::INFO, "Disconnecting unregistered client");
        clients_.erase(uuid);
        return ;
    }

    if ((*client)->GetType() == IClient::Type::kLocalUser || (*client)->GetType() == IClient::Type::kRemoteUser)
    {
		auto user = dynamic_cast<IUser*>(*client);				
        DisconnectUser(user, quit_message);
        return ;
    }

    if ((*client)->GetType() == IClient::Type::kLocalServer || (*client)->GetType() == IClient::Type::kRemoteServer)
    {
        DisconnectServer(dynamic_cast<IServer *>(*client), quit_message);
        return ;
    }
}

auto ClientDatabase::DisconnectUser(IUser *user,
	std::optional<std::string> quit_message) -> void
{
    auto user_uuid = user->GetUUID();

	if (!quit_message)
		quit_message = std::make_optional<std::string>("\"leaving\"");
	// Broadcast to user channels.
	auto channels = user->GetChannels();
	for (auto it = channels.cbegin(); it != channels.cend(); ++it)
	{
		it->second->PushToLocal(":" + user->GetPrefix() +
			" QUIT :" + *quit_message, user->GetUUID());
	}

	// Broadcast to all servers except local server is the user is remote.
	if (user->GetType() == IClient::Type::kRemoteUser)
	{
		BroadcastToLocalServers(":" + user->GetPrefix() +
			" QUIT :" + *quit_message, user->GetLocalServer()->GetUUID());
	}
	else
	{
		BroadcastToLocalServers(":" + user->GetPrefix() +
			" QUIT :" + *quit_message, std::nullopt);
	}

    logger.Log(LogLevel::INFO, "User with nickname: %s being disconnected", user->GetNickname().c_str());
    user->RemoveUserFromAllChannels();
    local_users_.erase(user_uuid);
    remote_users_.erase(user_uuid);
}

auto ClientDatabase::DisconnectServer(IServer *server,
	std::optional<std::string> quit_message) -> void
{
    logger.Log(LogLevel::INFO, "Server with name: %s being disconnected", server->GetServerName().c_str());
    server->Disconnect(this, quit_message);

	auto ignore_uuid = server->GetUUID();
	if (server->GetType() == IClient::Type::kRemoteServer)
		ignore_uuid = server->GetLocalServer()->GetUUID();

	this->BroadcastToLocalServers(":" + server_config_->GetName() + " SQUIT " + 
		server->GetServerName() + " :" + *quit_message, ignore_uuid);
    servers_.erase(server->GetUUID());
}

auto ClientDatabase::HandlePoll(std::unordered_map<IRC::UUID, std::unique_ptr<IClient>> &clients, 
    std::function<void(IClient*, std::string)> &message_handler) -> void
{
    for (auto it = clients.begin(), next_it = it; it != clients.end(); it = next_it)
    {
        ++next_it;
        try {
            std::optional<std::string> irc_message;

            /* Only receive one message each iteration. If we do more we can crash if we QUIT clients in the meantime */
            irc_message = it->second->Receive();
            if (irc_message)
                message_handler(it->second.get(), *irc_message);
        }
        catch (IClient::Disconnected &ex)
        {
            DisconnectClient(it->first, std::make_optional<std::string>("Connection interrupted"));
        }
    }
}

auto ClientDatabase::PollClients(std::function<void(IClient*, std::string)> message_handler) -> void
{
    HandlePoll(clients_, message_handler);
    HandlePoll(local_users_, message_handler);
    HandlePoll(servers_, message_handler);
}

auto ClientDatabase::HandleSendAll(std::unordered_map<IRC::UUID, std::unique_ptr<IClient>> &clients) -> void
{
    for (auto it = clients.begin(), next_it = it; it != clients.end(); it = next_it)
    {
        ++next_it;
        try {
            it->second->SendAll();
        }
        catch (IClient::Disconnected &ex)
        {
            DisconnectClient(it->first, std::make_optional<std::string>("Connection interrupted"));
        }
    }
}

auto ClientDatabase::SendAll() -> void
{
    HandleSendAll(clients_);
    HandleSendAll(local_users_);
    HandleSendAll(servers_);
}

/* Note that this only works with REAL Client objects */
auto ClientDatabase::RegisterLocalUser(IRC::UUID uuid) -> IClient*
{
    auto stored_client = clients_.find(uuid);
    if (stored_client == clients_.end())
        throw ClientDatabase::UnableToRegister("Unable to find unregistered client");

    auto client = dynamic_cast<Client *>(stored_client->second.get());

    if (client->GetNickname() == "" || client->GetUsername() == "")
        throw ClientDatabase::UnableToRegister("Nickname or Username not set");

    /* The following is quite hacky... my apologies, but here's what's happening:
        We create a new LockelUser object from the old UnRegistered Client object
        We exchange the content of the unique pointer that is holding the (now moved) Client object
        with the new LocalUser object. We move the unique pointer with the LocalUser to
        our list with local_users_ and remove the empty pointer from the clients_ list.
    */
    logger.Log(LogLevel::DEBUG, "Registering local user %s", client->GetNickname().c_str());
    auto tmp_unique_client = std::move(stored_client->second);
    tmp_unique_client.reset(new LocalUser(std::move(*client)));
    auto new_local_user = local_users_.insert(std::make_pair(uuid, std::move(tmp_unique_client)));
    clients_.erase(uuid);

    auto local_user = dynamic_cast<IUser*>(new_local_user.first->second.get());
	local_user->CachePrefix(server_config_->GetName());
    BroadcastToLocalServers(local_user->GenerateNickMessage(server_config_->GetName()), std::nullopt);
    return new_local_user.first->second.get();
}

/* Note that this only works with REAL Client objects */
auto ClientDatabase::RegisterLocalServer(std::string server_name, IRC::UUID uuid) -> IClient*
{
    auto stored_client = clients_.find(uuid);
    if (stored_client == clients_.end())
        throw ClientDatabase::UnableToRegister("Unable to find unregistered client");

    auto client = dynamic_cast<Client *>(stored_client->second.get());

    /* The following is quite hacky... my apologies, but here's what's happening:
        We create a new LocalServer object from the old UnRegistered Client object
        We exchange the content of the unique pointer that is holding the (now moved) Client object
        with the new LocalServer object. We move the unique pointer with the LocalServer to
        our list with servers_ and remove the empty pointer from the clients_ list.
    */

    logger.Log(LogLevel::INFO, "Registering local server [%s]", server_name.c_str());
    auto tmp_unique_client = std::move(stored_client->second);
    tmp_unique_client.reset(new LocalServer(server_name, std::move(*client)));
	tmp_unique_client->SetOurToken(GenerateOurServerToken());
    auto new_server = servers_.insert(std::make_pair(uuid, std::move(tmp_unique_client)));
    clients_.erase(uuid);
    return new_server.first->second.get();
}

auto ClientDatabase::AddLocalUser(std::unique_ptr<ILocalUser> new_localuser) -> void
{
    logger.Log(LogLevel::DEBUG, "Added local user %s to database", new_localuser->GetNickname().c_str());
    if (local_users_.find(new_localuser->GetUUID()) != local_users_.end())
        throw ClientDatabase::DuplicateClient();

    local_users_.insert(std::make_pair(new_localuser->GetUUID(), std::move(new_localuser)));
}

auto ClientDatabase::AddRemoteUser(std::unique_ptr<IRemoteUser> new_remoteuser) -> void
{
    logger.Log(LogLevel::DEBUG, "Added remote user %s to database", new_remoteuser->GetNickname().c_str());
    if (remote_users_.find(new_remoteuser->GetUUID()) != remote_users_.end())
        throw ClientDatabase::DuplicateClient();

    remote_users_.insert(std::make_pair(new_remoteuser->GetUUID(), std::move(new_remoteuser)));
}

auto ClientDatabase::AddServer(std::unique_ptr<IServer> new_server) -> void
{
    logger.Log(LogLevel::INFO, "Added remote server %s to database", new_server->GetServerName().c_str());
    if (servers_.find(new_server->GetUUID()) != servers_.end())
        throw ClientDatabase::DuplicateClient();

    new_server->SetOurToken(GenerateOurServerToken());
    servers_.insert(std::make_pair(new_server->GetUUID(), std::move(new_server)));
}

static auto FindNickname(std::unordered_map<IRC::UUID, std::unique_ptr<IClient>> &clients, const std::string &nickname) ->
    std::optional<IClient*>
{
    for (auto it = clients.begin(); it != clients.end(); it++)
    {
        if (it->second->GetNickname() == nickname)
            return std::optional<IClient*>(it->second.get());
    }
    return std::nullopt;
}

auto ClientDatabase::GetClient(IRC::UUID uuid) -> std::optional<IClient*>
{
    auto client = clients_.find(uuid);
    if (client != clients_.end())
        return std::optional<IClient*>(client->second.get());
    client = local_users_.find(uuid);
    if (client != local_users_.end())
        return std::optional<IClient*>(client->second.get());
    client = remote_users_.find(uuid);
    if (client != remote_users_.end())
        return std::optional<IClient*>(client->second.get());
    client = servers_.find(uuid);
    if (client != servers_.end())
        return std::optional<IClient*>(client->second.get());
    return std::nullopt;
}

auto ClientDatabase::GetClient(const std::string &name) -> std::optional<IClient*>
{
    auto client = FindNickname(clients_, name);
    if (client)
        return client;
    client = FindNickname(local_users_, name);
    if (client)
        return client;
    client = FindNickname(remote_users_, name);
    if (client)
        return client;
    client = GetServer(name);
	if (client)
		return client;
    return std::nullopt;
}

auto ClientDatabase::GetServer(IRC::UUID uuid) -> std::optional<IServer*>
{
    auto server = GetClient(uuid);

    if (server && ((*server)->GetType() == IClient::Type::kLocalServer || (*server)->GetType() == IClient::Type::kRemoteServer))
        return std::optional<IServer*>(dynamic_cast<IServer*>((*server)));
    else
        return std::nullopt;
}

auto ClientDatabase::GetServer(const std::string &server_name) -> std::optional<IServer*>
{
    for (auto it = servers_.begin(), next_it = it; it != servers_.end(); it = next_it)
    {
        ++next_it;
        auto server = dynamic_cast<IServer*>(it->second.get());
        if (server->GetServerName() == server_name)
            return std::optional<IServer*>(server);
    }
    return std::nullopt;
}

auto ClientDatabase::GetServer(const uint32_t token) -> std::optional<IServer*>
{
    for (auto it = servers_.begin(), next_it = it; it != servers_.end(); it = next_it)
    {
        ++next_it;
        auto server = dynamic_cast<IServer*>(it->second.get());
        if (server->GetTheirToken() == token)
            return std::optional<IServer*>(server);
    }
    return std::nullopt;
}

auto ClientDatabase::GetUser(const std::string &nickname) -> std::optional<IUser*>
{
    auto user = GetClient(nickname);

    if (user == std::nullopt)
    {
        logger.Log(LogLevel::DEBUG, "Couldn't find client %s", nickname.c_str());
        return std::nullopt;
    }
    if ((*user)->GetType() != IClient::Type::kLocalUser && (*user)->GetType() != IClient::Type::kRemoteUser)
    {
        logger.Log(LogLevel::DEBUG, "Client with nickname: %s not a user", nickname.c_str());
        return std::nullopt;
    }

    logger.Log(LogLevel::DEBUG, "Found client %s", nickname.c_str());
    return std::optional<IUser*>(dynamic_cast<IUser*>(*user));
}

auto ClientDatabase::SetConfig(IServerConfig *server_config) -> void
{
	server_config_ = server_config;
}

auto ClientDatabase::BroadcastToLocalUsers(const std::string &irc_message, std::optional<IRC::UUID> skip_uuid) -> void
{
    std::function<void(IClient*)> push_message = [irc_message](IClient* client)
    {
        client->Push(irc_message);
    };

    DoForEach(local_users_, push_message, skip_uuid);
}

auto ClientDatabase::BroadcastToLocalServers(const std::string &irc_message, std::optional<IRC::UUID> skip_uuid) -> void
{
    std::function<void(IClient*)> push_message = [irc_message](IClient* client)
    {
        if (client->GetType() == IClient::Type::kLocalServer)
        {
            client->Push(irc_message);
        }
    };

    DoForEach(servers_, push_message, skip_uuid);
}

auto ClientDatabase::DoForEachServer(std::function<void(IClient*)> action, std::optional<IRC::UUID> skip_uuid) -> void
{
    DoForEach(servers_, action, skip_uuid);
}

auto ClientDatabase::DoForEachUser(std::function<void(IClient*)> action, std::optional<IRC::UUID> skip_uuid) -> void
{
    DoForEach(local_users_, action, skip_uuid);
    DoForEach(remote_users_, action, skip_uuid);
}

auto ClientDatabase::DoForEachClient(std::function<void(IClient*)> action, std::optional<IRC::UUID> skip_uuid) -> void
{
    DoForEach(clients_, action, skip_uuid);
    DoForEach(servers_, action, skip_uuid);
    DoForEach(local_users_, action, skip_uuid);
    DoForEach(remote_users_, action, skip_uuid);
}

auto ClientDatabase::DoForEach(std::unordered_map<IRC::UUID, std::unique_ptr<IClient>> &clients, 
        std::function<void(IClient*)> &action,
        std::optional<IRC::UUID> &skip_uuid) -> void
{
    for (auto it = clients.begin(), next_it = it; it != clients.end(); it = next_it)
    {
        ++next_it;
        if (!skip_uuid || *skip_uuid != it->second->GetUUID())
            action(it->second.get());
    }
}

auto ClientDatabase::GenerateOurServerToken(void) -> uint32_t
{
	/* Token should always start at 2, since 0 is undefined and 1 is 'us'/current server. */
	uint32_t token = 2;

	for (;;)
	{
		if (!GetServer(token))
			break;
		token++;
	}

	return (token);
}
