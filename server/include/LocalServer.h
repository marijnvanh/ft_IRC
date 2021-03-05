#ifndef __LOCAL_SERVER_H__
#define __LOCAL_SERVER_H__

#include "IServer.h"
#include "UUID.h"

class LocalServer : public IServer
{
    public:
    LocalServer();
    ~LocalServer();

    /* Fake IClient functionality */
    auto Receive() -> std::optional<std::string> override { return std::nullopt; };
    auto Push(std::string irc_message) -> void override { (void)irc_message; };
    auto SendAll() -> void override {};
    auto GetUUID() const -> const IRC::UUID& override { return uuid_; };
    auto GetServer() -> IServer* override { return nullptr; };

    /* Fake IServer functionality */
    auto AddClient(IClient* client) -> void override;
    auto RemoveClient(IRC::UUID uuid) -> void override;

    private:
    IRC::UUID uuid_;
    std::unordered_map<IRC::UUID, IClient*> clients_;
};

#endif
