#ifndef _CLIENT_DATABASE_H__
#define _CLIENT_DATABASE_H__

#include <unordered_map>
#include <functional>

#include "IClient.h"
#include "Mutex.h"

class ClientDatabase
{
    public:

    ClientDatabase();
    ~ClientDatabase();

    auto AddClient(std::unique_ptr<IClient> new_client) -> void;
    auto RemoveClient(int UUID) -> void;

    /**
     * @brief Try to receive from all clients and call callback for each received message
     * 
     * @param message_handler 
     */
    auto PollClients(std::function<void(std::string)> message_handler) -> void;

    const int &operator [](int UUID);

    class ClientNotFound : public std::runtime_error
    {
    public:
        ClientNotFound() : std::runtime_error("Client not found in Database") {}
    };

    class DuplicateClient : public std::runtime_error
    {
    public:
        DuplicateClient() : std::runtime_error("Client already in database") {}
    };

    private:
    std::unordered_map<int, IRC::Mutex<IClient>> clients_;
};

#endif
