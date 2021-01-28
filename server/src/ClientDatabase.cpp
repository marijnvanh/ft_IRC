#include <ClientDatabase.h>
#include <optional>

ClientDatabase::ClientDatabase()
{}

ClientDatabase::~ClientDatabase()
{}

auto ClientDatabase::AddClient(std::unique_ptr<IClient> new_client) -> void
{
    auto uuid = new_client->GetUUID();
    IRC::Mutex<IClient> client_mutex(std::move(new_client));

    auto pair = std::make_pair(uuid, std::move(client_mutex));
    auto ret = clients_.insert(std::move(pair));

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
    std::unordered_map<int, IRC::Mutex<IClient>>::iterator it = clients_.begin();

    while (it != clients_.end())
    {
        try {
            auto client_handle = it->second.Take();
            std::optional<std::string> irc_message = client_handle->Receive();
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
