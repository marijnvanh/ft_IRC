#include <ClientDatabase.h>
#include <optional>

#include "LocalUser.h"
#include "Server.h"
#include "Client.h"

ClientDatabase::ClientDatabase() :
    clients_(IRC::MakeMutex<std::unordered_map<IRC::UUID, std::shared_ptr<IRC::Mutex<IClient>>>>())
{}

ClientDatabase::~ClientDatabase()
{}

auto ClientDatabase::AddClient(std::unique_ptr<IClient> new_client) -> void
{
    auto uuid = new_client->GetUUID();
    auto client = std::make_shared<IRC::Mutex<IClient>>(std::move(new_client));

    auto ret = clients_.Take()->insert(std::make_pair(uuid, client));

    /* Check if duplicate was found */
    if (ret.second == false)
        throw DuplicateClient();
}

auto ClientDatabase::RemoveClient(IRC::UUID uuid) -> void
{
    clients_.Take()->erase(uuid);
}

auto ClientDatabase::PollClients(std::function<void(std::shared_ptr<IRC::Mutex<IClient>>, std::string)> message_handler) -> void
{
    //TODO This might crash if other threads are deleting clients while we're in this loop
    for (auto it = clients_.Take()->begin(), next_it = it;
            it != clients_.Take()->end(); it = next_it)
    {
        ++next_it;
        try {
            std::optional<std::string> irc_message;
            while ((irc_message = it->second->Take()->Receive())) {
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

auto ClientDatabase::GetClient(IRC::UUID uuid) -> std::shared_ptr<IRC::Mutex<IClient>>
{
    auto client = clients_.Take()->find(uuid);
    if (client == clients_.Take()->end())
        throw ClientNotFound();
    return client->second;
}

auto ClientDatabase::SendAll() -> void
{
    clients_.Access([](auto &clients) {
        for (auto it = clients.begin(), next_it = it; it != clients.end(); it = next_it)
        {
            ++next_it;
            try {
                it->second->Take()->SendAll();
            }
            catch (IClient::Disconnected &ex)
            {
                //TODO Handle disconnection
                clients.erase(it->first);
            }
        }

    });
}
auto ClientDatabase::Find(const std::string &nickname) -> std::optional<std::shared_ptr<IRC::Mutex<IClient>>>
{
    auto clients = clients_.Take();

    for (auto it = clients->begin(), next_it = it; it != clients->end(); it = next_it)
    {
        ++next_it;
        if (it->second->Take()->GetNickname() == nickname)
            return std::optional<std::shared_ptr<IRC::Mutex<IClient>>>(it->second);
    }
    return std::nullopt;
}

auto ClientDatabase::RegisterLocalUser(IRC::UUID uuid) -> void
{
    try {
        auto client = dynamic_cast<Client *>(GetClient(uuid)->Take().Get()); // Ugh

        if (client->GetState() != IClient::State::kUnRegistered)
            throw ClientDatabase::UnAbleToRegister("Client not in a UnRegistered state");

        if (client->GetNickname() == "" || client->GetUsername() == "")
            throw ClientDatabase::UnAbleToRegister("Nickname or Username not set");

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
        auto client = dynamic_cast<Client *>(GetClient(uuid)->Take().Get()); // Ugh

        if (client->GetState() != IClient::State::kUnRegistered)
            throw ClientDatabase::UnAbleToRegister("Client not in a UnRegistered state");

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

//TODO server name ?
auto ClientDatabase::GetServer(std::string &server_name) -> std::optional<std::shared_ptr<IServer>>
{
    auto server = servers_.find(server_name);
    if (server != servers_.end())
        return std::optional<std::shared_ptr<IServer>>(server->second);
    else
        return std::nullopt;
}
