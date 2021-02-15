#include <ClientDatabase.h>
#include <optional>

ClientDatabase::ClientDatabase()
{}

ClientDatabase::~ClientDatabase()
{}

auto ClientDatabase::AddClient(std::unique_ptr<IClient> new_client) -> void
{
    auto uuid = new_client->GetUUID();
    auto client = std::make_shared<IRC::Mutex<IClient>>(std::move(new_client));

    auto ret = clients_.insert(std::make_pair(uuid, client));

    /* Check if duplicate was found */
    if (ret.second == false)
        throw DuplicateClient();
}

auto ClientDatabase::RemoveClient(IRC::UUID uuid) -> void
{
    clients_.erase(uuid);
}

auto ClientDatabase::PollClients(std::function<void(IRC::UUID uuid, std::string)> message_handler) -> void
{
    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        try {
            std::optional<std::string> irc_message;
            it->second->Access([&irc_message](IClient &client)
            {
                irc_message = client.Receive();
            });
            
            if (irc_message)
                message_handler(it->first, *irc_message);
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
    auto client = clients_.find(uuid);
    if (client == clients_.end())
        throw ClientNotFound();
    return client->second;
}

auto ClientDatabase::SendAll() -> void
{
    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        try {
            it->second->Access([](IClient &client)
            {
                client.SendAll();
            });
        }
        catch (IClient::Disconnected &ex)
        {
            //TODO Handle disconnection
            RemoveClient(it->first);
        }
    }
}
