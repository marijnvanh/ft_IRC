#include <ClientDatabase.h>
#include <optional>

#include "LocalUser.h"
#include "Server.h"
#include "Client.h"

ClientDatabase::ClientDatabase()
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

auto ClientDatabase::RemoveClient(IRC::UUID uuid) -> void
{
    clients_.erase(uuid);
}

auto ClientDatabase::RemoveUser(IRC::UUID uuid) -> void
{
    local_users_.erase(uuid);
    remote_users_.erase(uuid);
}

auto ClientDatabase::RemoveServer(IRC::UUID uuid) -> void
{
    servers_.erase(uuid);
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
            //TODO Handle disconnection
            RemoveClient(it->first);
        }
    }
}

auto ClientDatabase::PollClients(std::function<void(IClient*, std::string)> message_handler) -> void
{
    HandlePoll(clients_, message_handler);
    HandlePoll(local_users_, message_handler);
    HandlePoll(remote_users_, message_handler);
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
            //TODO Handle disconnection
            RemoveClient(it->first);
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
auto ClientDatabase::RegisterLocalUser(IRC::UUID uuid) -> void
{
    (void)uuid;
    // auto new_local_user = GetClient(uuid);
    // if (new_local_user == std::nullopt)
    //     return ;

    // if ((*new_local_user)->GetState() != IClient::State::kUnRegistered)
    //     throw ClientDatabase::UnableToRegister("Client not in a UnRegistered state");

    // if ((*new_local_user)->GetNickname() == "" || (*new_local_user)->GetUsername() == "")
    //     throw ClientDatabase::UnableToRegister("Nickname or Username not set");

    // /* The following is rather hacky... but here's what's happening:
    //     We create a new LockelUser object from the old UnRegistered Client object
    //     We exchange the content of the shared pointer that is holding the (now moved) Client object
    //     with the new LocalUser object. We add the shared pointer with the LocalUser to
    //     our list with local_users_ and remove the shared pointer from the clients_ list.
    // */

    // auto raw_client = dynamic_cast<Client *>(*new_local_user);
    // std::unique_ptr<IClient> tmp_local_user = std::make_unique<LocalUser>(std::move(*raw_client));
    // new_local_user->swap(tmp_local_user);
    // local_users_.insert(std::make_pair(uuid, *new_local_user));
    // RemoveClient(uuid);

    // auto server = (*new_local_user)->GetServer();
    // server->AddClient(*new_local_user);
}

/* Note that this only works with REAL Client objects */
//TODO add check if server already exists ?
auto ClientDatabase::RegisterServer(IRC::UUID uuid) -> void
{
    (void)uuid;
    // auto new_server = GetClient(uuid);
    // if (new_server == std::nullopt)
    //     return ;

    // if ((*new_server)->GetState() != IClient::State::kUnRegistered)
    //     throw ClientDatabase::UnableToRegister("Client not in a UnRegistered state");

    // /* The following is rather hacky... but here's what's happening:
    //     We create a new server object from the old UnRegistered Client object
    //     We exchange the content of the shared pointer that is holding the (now moved) Client object
    //     with the new server object. We add the shared pointer with the server to
    //     our list with servers_ and remove the shared pointer from the clients_ list.
    // */
    // auto raw_client = dynamic_cast<Client *>((*new_server).get());

    // std::unique_ptr<IClient> tmp_server = std::make_shared<LocalUser>(std::move(*raw_client));
    // new_server->swap(tmp_server);
    // servers_.insert(std::make_pair(uuid, *new_server));
    // RemoveClient(uuid);
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
auto ClientDatabase::GetServer(std::string &server_name) -> std::optional<IServer*>
{
    for (auto it = servers_.begin(), next_it = it; it != servers_.end(); it = next_it)
    {
        ++next_it;
        if (it->second->GetNickname() == server_name)
            return std::optional<IServer*>(dynamic_cast<IServer*>(it->second.get()));
    }
    return std::nullopt;
}
