#ifndef _CLIENT_DATABASE_H__
#define _CLIENT_DATABASE_H__

#include <unordered_map>
#include <functional>

#include "IClient.h"
#include "Mutex.h"
#include "UUID.h"

class ClientDatabase
{
    public:

    ClientDatabase();
    ~ClientDatabase();

    /**
     * @brief Add client to ClientDatabase
     * 
     * @exception DuplicateClient if client already exists
     * 
     * @param new_client 
     */
    auto AddClient(std::unique_ptr<IClient> new_client) -> void;

    /**
     * @brief Remove client from database
     * If no client with uuid exists it does nothing
     * 
     * @param uuid 
     */
    auto RemoveClient(int uuid) -> void;

    /**
     * @brief Try to receive from all clients and call callback for each received message
     * 
     * @param message_handler 
     */
    auto PollClients(std::function<void(int, std::string)> message_handler) -> void;

    /**
     * @brief Empty all client send queus
     * 
     */
    auto SendAll() -> void;

    auto GetClient(int uuid) -> std::shared_ptr<IRC::Mutex<IClient>>;

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
    std::unordered_map<int, std::shared_ptr<IRC::Mutex<IClient>>> clients_;
};

#endif
