#ifndef __REMOTE_SERVER_H__
#define __REMOTE_SERVER_H__

#include "Server.h"
#include "UUID.h"

class RemoteServer : public Server
{
    public:
    RemoteServer(IServer* local_server, IServer* remote_server, std::string server_name);
    ~RemoteServer();

    /* Fake IClient functionality */
    auto Receive() -> std::optional<std::string> override { return std::nullopt; };
    auto SendAll() -> void override {};

    /* Functional IClient functionality */
    auto GetUUID() const -> const IRC::UUID& override { return uuid_; };
    auto Push(std::string irc_message) -> void override;
 
    private:
    std::unordered_map<IRC::UUID, IClient*> clients_;
    IRC::UUID uuid_;
};

#endif
