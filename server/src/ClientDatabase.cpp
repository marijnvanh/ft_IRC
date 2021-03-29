#include <ClientDatabase.h>
#include <optional>

#include "LocalUser.h"
#include "Server.h"
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

auto ClientDatabase::DisconnectClient(IRC::UUID uuid) -> void
{
    auto client = GetClient(uuid);
    if (!client)
    {
        logger.Log(LogLevel::ERROR, "No client to delete"); // Should never happen
        return;
    }
    
    if ((*client)->GetState() == IClient::State::kUnRegistered)
    {
        logger.Log(LogLevel::INFO, "Client with nickname: %s being disconnected", (*client)->GetNickname().c_str());
        clients_.erase(uuid);
        return ;
    }

    if ((*client)->GetType() == IClient::Type::kLocalUser || (*client)->GetType() == IClient::Type::kRemoteUser)
    {
        DisconnectUser(dynamic_cast<IUser *>(*client));
        return ;
    }

    if ((*client)->GetType() == IClient::Type::kServer)
    {
        DisconnectServer(dynamic_cast<IServer *>(*client));
        return ;
    }
}

auto ClientDatabase::DisconnectUser(IUser *user) -> void
{
    auto user_uuid = user->GetUUID();
    logger.Log(LogLevel::INFO, "User with nickname: %s being disconnected", user->GetNickname().c_str());
    user->RemoveUserFromAllChannels();
    local_users_.erase(user_uuid);
    remote_users_.erase(user_uuid);
}

auto ClientDatabase::DisconnectServer(IServer *server) -> void
{
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
            //TODO Handle quit message sending
            DisconnectClient(it->first);
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
            //TODO Handle quit message sending
            DisconnectClient(it->first);
        }
    }
}

auto ClientDatabase::SendAll() -> void
{
    HandleSendAll(clients_);
    HandleSendAll(local_users_);
    HandleSendAll(remote_users_);
    HandleSendAll(servers_);
}

/* Note that this only works with REAL Client objects */
//TODO add check if nickname already exists ?
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
    logger.Log(LogLevel::DEBUG, "Registering %s", client->GetNickname().c_str());
    auto tmp_unique_client = std::move(stored_client->second);
    tmp_unique_client.reset(new LocalUser(std::move(*client)));
    auto new_local_user = local_users_.insert(std::make_pair(uuid, std::move(tmp_unique_client)));
    clients_.erase(uuid);
    return new_local_user.first->second.get();
}

/* Note that this only works with REAL Client objects */
//TODO add check if server already exists ?
auto ClientDatabase::RegisterServer(IRC::UUID uuid) -> IClient*
{
    auto stored_client = clients_.find(uuid);
    if (stored_client == clients_.end())
        throw ClientDatabase::UnableToRegister("Unable to find unregistered client");

    auto client = dynamic_cast<Client *>(stored_client->second.get());

    /* The following is quite hacky... my apologies, but here's what's happening:
        We create a new Server object from the old UnRegistered Client object
        We exchange the content of the unique pointer that is holding the (now moved) Client object
        with the new Server object. We move the unique pointer with the Server to
        our list with servers_ and remove the empty pointer from the clients_ list.
    */

    auto tmp_unique_client = std::move(stored_client->second);
    tmp_unique_client.reset(new Server(std::move(*client)));
    auto new_server = servers_.insert(std::make_pair(uuid, std::move(tmp_unique_client)));
    clients_.erase(uuid);
    return new_server.first->second.get();
}

auto ClientDatabase::AddLocalUser(std::unique_ptr<ILocalUser> new_localuser) -> void
{
    if (local_users_.find(new_localuser->GetUUID()) != local_users_.end())
        throw ClientDatabase::DuplicateClient();

    local_users_.insert(std::make_pair(new_localuser->GetUUID(), std::move(new_localuser)));
}

auto ClientDatabase::AddRemoteUser(std::unique_ptr<IRemoteUser> new_remoteuser) -> void
{
    if (remote_users_.find(new_remoteuser->GetUUID()) != remote_users_.end())
        throw ClientDatabase::DuplicateClient();

    remote_users_.insert(std::make_pair(new_remoteuser->GetUUID(), std::move(new_remoteuser)));
}

auto ClientDatabase::AddServer(std::unique_ptr<IServer> new_server) -> void
{
    if (servers_.find(new_server->GetUUID()) != servers_.end())
        throw ClientDatabase::DuplicateClient();

    servers_.insert(std::make_pair(new_server->GetUUID(), std::move(new_server)));
}

auto ClientDatabase::GetClient(IRC::UUID uuid) -> std::optional<IClient*>
{
    auto client = clients_.find(uuid);
    if (client != clients_.end())
        return std::optional<IClient*>(client->second.get());
    client = local_users_.find(uuid);
    if (client != clients_.end())
        return std::optional<IClient*>(client->second.get());
    client = remote_users_.find(uuid);
    if (client != clients_.end())
        return std::optional<IClient*>(client->second.get());
    client = servers_.find(uuid);
    if (client != clients_.end())
        return std::optional<IClient*>(client->second.get());
    return std::nullopt;
}

static auto FindNickname(std::unordered_map<IRC::UUID, std::unique_ptr<IClient>> &clients, const std::string &nickname) ->
    std::optional<IClient*>
{
    for (auto it = clients.begin(), next_it = it; it != clients.end(); it = next_it)
    {
        ++next_it;
        if (it->second->GetNickname() == nickname)
            return std::optional<IClient*>(it->second.get());
    }
    return std::nullopt;
}

auto ClientDatabase::GetClient(const std::string &nickname) -> std::optional<IClient*>
{
    auto client = FindNickname(clients_, nickname);
    if (client)
        return client;
    client = FindNickname(local_users_, nickname);
    if (client)
        return client;
    client = FindNickname(remote_users_, nickname);
    if (client)
        return client;
    client = FindNickname(servers_, nickname);
    if (client)
        return client;
    return std::nullopt;
}

//TODO server name ?
auto ClientDatabase::GetServer(const std::string &server_name) -> std::optional<IServer*>
{
    for (auto it = servers_.begin(), next_it = it; it != servers_.end(); it = next_it)
    {
        ++next_it;
        if (it->second->GetNickname() == server_name)
            return std::optional<IServer*>(dynamic_cast<IServer*>(it->second.get()));
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
    if ((*user)->GetType() != IClient::kLocalUser && (*user)->GetType() != IClient::kRemoteUser)
    {
        logger.Log(LogLevel::DEBUG, "Client with nickname: %s not a user", nickname.c_str());
        return std::nullopt;
    }

    logger.Log(LogLevel::DEBUG, "Found client %s", nickname.c_str());
    return std::optional<IUser*>(dynamic_cast<IUser*>(*user));
}
