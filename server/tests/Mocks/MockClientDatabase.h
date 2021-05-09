#ifndef MOCK_CLIENT_DATABASE_H__
#define MOCK_CLIENT_DATABASE_H__

#include <string>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IClientDatabase.h"

class MockClientDatabase : public IClientDatabase {
    public:

    MOCK_METHOD(IClient*, AddClient, (std::unique_ptr<IClient> new_client), ());
    MOCK_METHOD(void, AddLocalUser, (std::unique_ptr<ILocalUser> new_localuser), ());
    MOCK_METHOD(void, AddRemoteUser, (std::unique_ptr<IRemoteUser> new_remoteuser), ());
    MOCK_METHOD(void, AddServer, (std::unique_ptr<IServer> new_server), ());
    MOCK_METHOD(void, DisconnectClient, (IRC::UUID uuid, std::optional<std::string>), ());
    MOCK_METHOD(void, DisconnectUser, (IUser *user, std::optional<std::string>), ());
    MOCK_METHOD(void, DisconnectServer, (IServer *server, std::optional<std::string>), ());
    MOCK_METHOD(std::optional<IClient*>, GetClient, (IRC::UUID uuid), ());
    MOCK_METHOD(std::optional<IClient*>, GetClient, (const std::string &nickname), ());
    MOCK_METHOD(IClient*, RegisterLocalUser, (IRC::UUID uuid), ());
    MOCK_METHOD(IClient*, RegisterLocalServer, (std::string server_name, IRC::UUID uuid), ());
    MOCK_METHOD(std::optional<IServer*>, GetServer, (const std::string &server_name), ());
    MOCK_METHOD(std::optional<IServer*>, GetServer, (IRC::UUID uuid), ());
    MOCK_METHOD(std::optional<IUser*>, GetUser, (const std::string &nickname), ());
    MOCK_METHOD(void, BroadcastToLocalUsers, (const std::string &irc_message, std::optional<IRC::UUID> skip_uuid), ());
    MOCK_METHOD(void, BroadcastToLocalServers, (const std::string &irc_message, std::optional<IRC::UUID> skip_uuid), ());
    MOCK_METHOD(void, DoForEachServer, (std::function<void(IClient*)> action, std::optional<IRC::UUID> skip_uuid), ());
    MOCK_METHOD(void, DoForEachUser, (std::function<void(IClient*)> action, std::optional<IRC::UUID> skip_uuid), ());
    MOCK_METHOD(void, DoForEachClient, (std::function<void(IClient*)> action, std::optional<IRC::UUID> skip_uuid), ());

};

#endif
