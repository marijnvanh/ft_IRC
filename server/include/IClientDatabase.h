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

    virtual auto AddClient(std::unique_ptr<IClient> new_client) -> IClient* = 0;
    virtual auto AddLocalUser(std::unique_ptr<ILocalUser> new_localuser) -> void = 0;
    virtual auto AddRemoteUser(std::unique_ptr<IRemoteUser> new_remoteuser) -> void = 0;
    virtual auto AddServer(std::unique_ptr<IServer> new_server) -> void = 0;
    virtual auto RemoveClient(IRC::UUID uuid) -> void = 0;
    virtual auto RemoveUser(IRC::UUID uuid) -> void = 0;
    virtual auto RemoveServer(IRC::UUID uuid) -> void = 0;
    virtual auto GetClient(IRC::UUID uuid) -> std::optional<IClient*> = 0;
    virtual auto GetClient(const std::string &nickname) -> std::optional<IClient*> = 0;
    virtual auto GetServer(const std::string &server_name) -> std::optional<IServer*> = 0;
    virtual auto GetUser(const std::string &nickname) -> std::optional<IUser*> = 0;
    virtual auto RegisterLocalUser(IRC::UUID uuid) -> IClient* = 0;
    virtual auto RegisterServer(IRC::UUID uuid) -> IClient* = 0;

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
