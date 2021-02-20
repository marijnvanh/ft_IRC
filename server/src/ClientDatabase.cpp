#include <ClientDatabase.h>
#include <optional>

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

auto ClientDatabase::PollClients(std::function<void(IRC::UUID uuid, std::string)> message_handler) -> void
{
    //TODO This might crash if other threads are deleting clients while we're in this loop
    for (auto it = clients_.Take()->begin(), next_it = it;
            it != clients_.Take()->end(); it = next_it)
    {
        ++next_it;
        try {
            std::optional<std::string> irc_message;
            while ((irc_message = it->second->Take()->Receive())) {
                message_handler(it->first, *irc_message);
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
