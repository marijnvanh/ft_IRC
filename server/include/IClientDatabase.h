#ifndef _ICLIENT_DATABASE_H__
#define _ICLIENT_DATABASE_H__

#include <unordered_map>
#include <functional>
#include <optional>

#include "IClient.h"
#include "UUID.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"
#include "IServer.h"

class IClientDatabase
{
    public:

    virtual ~IClientDatabase() {};

    virtual auto AddClient(std::unique_ptr<IClient> new_client) -> std::shared_ptr<IClient> = 0;
    virtual auto AddLocalUser(std::shared_ptr<ILocalUser> new_localuser) -> void = 0;
    virtual auto AddRemoteUser(std::shared_ptr<IRemoteUser> new_remoteuser) -> void = 0;
    virtual auto AddServer(std::shared_ptr<IServer> new_server) -> void = 0;
    virtual auto RemoveClient(IRC::UUID uuid) -> void = 0;
    virtual auto RemoveUser(IRC::UUID uuid) -> void = 0;
    virtual auto RemoveServer(IRC::UUID uuid) -> void = 0;
    virtual auto GetClient(IRC::UUID uuid) -> std::optional<std::shared_ptr<IClient>> = 0;
    virtual auto GetClient(const std::string &nickname) -> std::optional<std::shared_ptr<IClient>> = 0;
    virtual auto GetServer(std::string &server_name) -> std::optional<std::shared_ptr<IServer>> = 0;
    virtual auto RegisterLocalUser(IRC::UUID uuid) -> void = 0;
    virtual auto RegisterServer(IRC::UUID uuid) -> void = 0;

    class DuplicateClient : public std::runtime_error
    {
    public:
        DuplicateClient() : std::runtime_error("Client already in database") {}
    };

    class UnableToRegister : public std::runtime_error
    {
    public:
        UnableToRegister(std::string err) : std::runtime_error(err) {}
    };
};

#endif
