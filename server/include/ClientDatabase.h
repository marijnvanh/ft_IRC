#ifndef _CLIENT_DATABASE_H__
#define _CLIENT_DATABASE_H__

#include <string>
#include "IClientDatabase.h"

class ClientDatabase : public IClientDatabase
{
    public:

    ClientDatabase();
    ~ClientDatabase();

    /**
     * @brief Add client type to ClientDatabase
     * 
     * @exception DuplicateClient if client already exists
     * 
     * @param new_client 
     */
    auto AddClient(std::unique_ptr<IClient> new_client) -> void override;
    auto AddLocalUser(std::shared_ptr<ILocalUser> new_localuser) -> void override;
    auto AddRemoteUser(std::shared_ptr<IRemoteUser> new_remoteuser) -> void override;
    auto AddServer(std::shared_ptr<IServer> new_server) -> void override;

    /**
     * @brief Register functions to upgrade a client to a localuser or server 
     * 
     * @param uuid 
     */
    auto RegisterLocalUser(IRC::UUID uuid) -> void override;
    auto RegisterServer(IRC::UUID uuid) -> void override;

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
    auto PollClients(std::function<void(std::shared_ptr<IRC::Mutex<IClient>>, std::string)> message_handler) -> void;

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

    /**
     * @brief Search client database for nickname
     * 
     * @param nickname 
     * @return is a client if client with nickname is found
     */
    auto Find(const std::string &nickname) -> std::optional<std::shared_ptr<IRC::Mutex<IClient>>> override;

    private:
    IRC::Mutex<std::unordered_map<IRC::UUID, std::shared_ptr<IRC::Mutex<IClient>>>> clients_;
    std::unordered_map<std::string, std::shared_ptr<ILocalUser>> local_users_;
    std::unordered_map<std::string, std::shared_ptr<IRemoteUser>> remote_users_;
    std::unordered_map<std::string, std::shared_ptr<IServer>> servers_;
};

#endif
