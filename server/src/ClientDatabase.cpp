#include <ClientDatabase.h>
#include <optional>

ClientDatabase::ClientDatabase()
{}

ClientDatabase::~ClientDatabase()
{}

auto ClientDatabase::AddClient(std::unique_ptr<IClient> new_client) -> void
{
    std::pair<std::unordered_map<int, std::unique_ptr<IClient>>::iterator,bool> ret;

    ret = clients_.insert(std::make_pair(new_client->GetUUID(), std::move(new_client)));

    /* Check if duplicate was found */
    if (ret.second == false)
        throw DuplicateClient();
}

auto ClientDatabase::RemoveClient(int UUID) -> void
{
    clients_.erase(UUID);
}

auto ClientDatabase::PollClients(std::function<void(std::string)> message_handler) -> void
{
    std::unordered_map<int, std::unique_ptr<IClient>>::iterator it = clients_.begin();

    while (it != clients_.end())
    {
        try {
            std::optional<std::string> irc_message = it->second->Receive();
            if (irc_message)
                message_handler(*irc_message);
        }
        catch (IClient::Disconnected &ex)
        {
            //TODO Handle disconnection
            RemoveClient(it->first);
        }
        it++;
    }
}

const int &ClientDatabase::operator [](int UUID)
{
    auto client = clients_.find(UUID);
    if (client == clients_.end())
        throw ClientNotFound();
    return client->first;
}
