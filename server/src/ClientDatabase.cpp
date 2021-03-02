#include <ClientDatabase.h>
#include <optional>

#include "LocalUser.h"
#include "Server.h"
#include "Client.h"

ClientDatabase::ClientDatabase()
{}

ClientDatabase::~ClientDatabase()
{}

auto ClientDatabase::AddClient(std::unique_ptr<IClient> new_client) -> void
{
    std::shared_ptr<IClient> client = std::move(new_client);
    auto ret = clients_.insert(std::make_pair(client->GetUUID(), client));

    /* Check if duplicate was found */
    if (ret.second == false)
        throw DuplicateClient();
}

auto ClientDatabase::RemoveClient(IRC::UUID uuid) -> void
{
    clients_.erase(uuid);
}

auto ClientDatabase::RemoveUser(const std::string &nickname) -> void
{
    local_users_.erase(nickname);
    remote_users_.erase(nickname);
}

auto ClientDatabase::RemoveServer(const std::string &server_name) -> void
{
    servers_.erase(server_name);
}

auto ClientDatabase::PollClients(std::function<void(std::shared_ptr<IClient>, std::string)> message_handler) -> void
{
    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        try {
            std::optional<std::string> irc_message;
            while ((irc_message = it->second->Receive())) {
                message_handler(it->second, *irc_message);
            }
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
    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        try {
            it->second->SendAll();
        }
        catch (IClient::Disconnected &ex)
        {
            //TODO Handle disconnection
            clients_.erase(it->first);
        }
    }
}

auto ClientDatabase::RegisterLocalUser(IRC::UUID uuid) -> void
{
    try {
        auto client = dynamic_cast<Client *>(GetClient(uuid).get());

        if (client->GetState() != IClient::State::kUnRegistered)
            throw ClientDatabase::UnableToRegister("Client not in a UnRegistered state");

        if (client->GetNickname() == "" || client->GetUsername() == "")
            throw ClientDatabase::UnableToRegister("Nickname or Username not set");

        auto local_user = std::make_shared<LocalUser>(std::move(*client));
        local_users_.insert(std::make_pair(local_user->GetNickname(), local_user));

        RemoveClient(uuid);
    } catch (ClientDatabase::ClientNotFound &ex) {
        return ;
    }
}

auto ClientDatabase::RegisterServer(IRC::UUID uuid) -> void
{
    try {
        auto client = dynamic_cast<Client *>(GetClient(uuid).get());

        if (client->GetState() != IClient::State::kUnRegistered)
            throw ClientDatabase::UnableToRegister("Client not in a UnRegistered state");

        auto server = std::make_shared<Server>(std::move(*client));
        servers_.insert(std::make_pair(server->GetNickname(), server));

        RemoveClient(uuid);
    } catch (ClientDatabase::ClientNotFound &ex) {
        return ;
    }
}

auto ClientDatabase::AddLocalUser(std::shared_ptr<ILocalUser> new_localuser) -> void
{
    if (local_users_.find(new_localuser->GetNickname()) != local_users_.end())
        throw ClientDatabase::DuplicateClient();

    local_users_.insert(std::make_pair(new_localuser->GetNickname(), new_localuser));
}

auto ClientDatabase::AddRemoteUser(std::shared_ptr<IRemoteUser> new_remoteuser) -> void
{
    if (remote_users_.find(new_remoteuser->GetNickname()) != remote_users_.end())
        throw ClientDatabase::DuplicateClient();

    remote_users_.insert(std::make_pair(new_remoteuser->GetNickname(), new_remoteuser));
}

auto ClientDatabase::AddServer(std::shared_ptr<IServer> new_server) -> void
{
    if (servers_.find(new_server->GetNickname()) != servers_.end())//TODO not by nickname ?
        throw ClientDatabase::DuplicateClient();

    servers_.insert(std::make_pair(new_server->GetNickname(), new_server));
}

auto ClientDatabase::GetClient(IRC::UUID uuid) -> std::shared_ptr<IClient>
{
    auto client = clients_.find(uuid);
    if (client == clients_.end())
        throw ClientNotFound();
    return client->second;
}

auto ClientDatabase::GetClient(const std::string &nickname) -> std::optional<std::shared_ptr<IClient>>
{
    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        if (it->second->GetNickname() == nickname)
            return std::optional<std::shared_ptr<IClient>>(it->second);
    }

    auto local_user = local_users_.find(nickname);
    if (local_user != local_users_.end())
        return std::optional<std::shared_ptr<IClient>>(local_user->second);
    
    auto remote_user = remote_users_.find(nickname);
    if (remote_user != remote_users_.end())
        return std::optional<std::shared_ptr<IClient>>(remote_user->second);

    return std::nullopt;
}

//TODO server name ?
auto ClientDatabase::GetServer(std::string &server_name) -> std::optional<std::shared_ptr<IServer>>
{
    auto server = servers_.find(server_name);
    if (server != servers_.end())
        return std::optional<std::shared_ptr<IServer>>(server->second);
    else
        return std::nullopt;
}
