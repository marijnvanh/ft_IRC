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
    auto AddClient(std::unique_ptr<IClient> new_client) -> std::shared_ptr<IClient> override;
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
    auto RemoveUser(IRC::UUID uuid) -> void override;
    auto RemoveServer(IRC::UUID uuid) -> void override;

    /**
     * @brief Try to receive from all clients and call callback for each received message
     * 
     * @param message_handler 
     */
    auto PollClients(std::function<void(std::shared_ptr<IClient>, std::string)> message_handler) -> void;

    /**
     * @brief Empty all client send queus
     * 
     */
    auto SendAll() -> void;

    /**
     * @brief Get client
     * 
     * @param uuid 
     */
    auto GetClient(IRC::UUID uuid) -> std::optional<std::shared_ptr<IClient>> override;
    
    // Get Client by nickname currently returns both Registered and UnRegistered users
    // This might not be what we want because of Nick collisions on UnRegistered users
    auto GetClient(const std::string &nickname) -> std::optional<std::shared_ptr<IClient>> override;
    auto GetServer(std::string &server_name) -> std::optional<std::shared_ptr<IServer>> override;

    private:

    auto HandlePoll(std::unordered_map<IRC::UUID, std::shared_ptr<IClient>> &clients, 
        std::function<void(std::shared_ptr<IClient>, std::string)> &message_handler) -> void;
    auto HandleSendAll(std::unordered_map<IRC::UUID, std::shared_ptr<IClient>> &clients) -> void;

    std::unordered_map<IRC::UUID, std::shared_ptr<IClient>> clients_;
    std::unordered_map<IRC::UUID, std::shared_ptr<IClient>> local_users_;
    std::unordered_map<IRC::UUID, std::shared_ptr<IClient>> remote_users_;
    std::unordered_map<IRC::UUID, std::shared_ptr<IClient>> servers_;
};

#endif
