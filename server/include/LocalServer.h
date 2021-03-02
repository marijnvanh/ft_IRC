#ifndef __LOCAL_SERVER_H__
#define __LOCAL_SERVER_H__

#include "IServer.h"
#include "UUID.h"

class LocalServer : public IServer
{
    public:
    LocalServer() : uuid_(IRC::UUIDGenerator::GetInstance().Generate()) {};
    ~LocalServer() {};

    /* Fake IClient functionality */
    auto Receive() -> std::optional<std::string> override { return std::nullopt; };
    auto Push(std::string irc_message) -> void override { (void)irc_message; };
    auto SendAll() -> void override {};
    auto GetUUID() const -> const IRC::UUID& override { return uuid_; };

    /* Fake IServer functionality */

    private:
    IRC::UUID uuid_;
};

#endif