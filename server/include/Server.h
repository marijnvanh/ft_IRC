#ifndef __SERVER_H__
#define __SERVER_H__

#include "IServer.h"

class Server : public virtual IServer
{
    public:
    Server(std::string server_name);
    virtual ~Server();

    auto AddClient(IClient* client) -> void override;
    auto RemoveClient(IRC::UUID uuid) -> void override;
    auto GetServerName() const -> const std::string& override { return server_name_; };
    auto Disconnect(IClientDatabase *client_database,
		std::optional<std::string> message) -> void override;

    /**
     * @brief 
     * 
     * @param this_server_name // Hacked this in here because IServer doesn't know about our server
     * @return std::string 
     */
    auto GenerateServerMessage(const std::string &this_server_name) const -> std::string override;

    private:
    std::unordered_map<IRC::UUID, IClient*> clients_;
    std::string server_name_;
};

#endif
