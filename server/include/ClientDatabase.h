#ifndef _CLIENT_DATABASE_H__
#define _CLIENT_DATABASE_H__

#include <unordered_map>
#include <functional>

#include "IClientDatabase.h"
#include "IClient.h"
#include "Mutex.h"
#include "UUID.h"

class ClientDatabase : public IClientDatabase
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
    auto AddClient(std::unique_ptr<IClient> new_client) -> void override;

    /**
     * @brief Remove client from database
     * If no client with uuid exists it does nothing
     * 
     * @param uuid 
     */
    auto RemoveClient(IRC::UUID uuid) -> void override;

    /**
     * @brief Try to receive from all clients and call callback for each received message
     * 
     * @param message_handler 
     */
    auto PollClients(std::function<void(IRC::UUID, std::string)> message_handler) -> void;

    /**
     * @brief Empty all client send queus
     * 
     */
    auto SendAll() -> void;

    /**
     * @brief Get the Client object
     * 
     * @exception ClientNotFound if client uuid is not in the database
     * @param uuid 
     * @return std::shared_ptr<IRC::Mutex<IClient>> 
     */
    auto GetClient(IRC::UUID uuid) -> std::shared_ptr<IRC::Mutex<IClient>> override;

    private:
    IRC::Mutex<std::unordered_map<IRC::UUID, std::shared_ptr<IRC::Mutex<IClient>>>> clients_;
};

#endif
