#include <ClientDatabase.h>
#include <optional>

ClientDatabase::ClientDatabase()
{}

ClientDatabase::~ClientDatabase()
{}

auto ClientDatabase::AddClient(std::unique_ptr<IClient> new_client) -> void
{
    auto uuid = new_client->GetUUID();

    auto ret = clients_.insert(std::make_pair(
        uuid,
        IRC::Mutex<IClient>(std::move(new_client)))
    );

    /* Check if duplicate was found */
    if (ret.second == false)
        throw DuplicateClient();
}

auto ClientDatabase::RemoveClient(int UUID) -> void
{
    clients_.erase(UUID);
}

auto ClientDatabase::PollClients(std::function<void(int, std::string)> message_handler) -> void
{
    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        try {
            std::optional<std::string> irc_message;
            it->second.Access([&irc_message](IClient &client)
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

auto ClientDatabase::operator [](int UUID) const -> int
{
    auto client = clients_.find(UUID);
    if (client == clients_.end())
        throw ClientNotFound();
    return client->first;
}

auto ClientDatabase::SendAll() -> void
{
    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        try {
            it->second.Access([](IClient &client)
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
