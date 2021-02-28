#ifndef MOCK_CLIENT_DATABASE_H__
#define MOCK_CLIENT_DATABASE_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IClientDatabase.h"

class MockClientDatabase : public IClientDatabase {
    public:

    MOCK_METHOD(void, AddClient, (std::unique_ptr<IClient> new_client), ());
    MOCK_METHOD(void, RemoveClient, (IRC::UUID uuid), ());
    MOCK_METHOD(std::shared_ptr<IRC::Mutex<IClient>>, GetClient, (IRC::UUID uuid), ());
    MOCK_METHOD(std::optional<std::shared_ptr<IRC::Mutex<IClient>>>, Find, (const std::string &nickname), ());
    MOCK_METHOD(void, AddLocalUser, (std::shared_ptr<ILocalUser> new_localuser), ());
    MOCK_METHOD(void, AddRemoteUser, (std::shared_ptr<IRemoteUser> new_remoteuser), ());
    MOCK_METHOD(void, AddServer, (std::shared_ptr<IServer> new_server), ());
    MOCK_METHOD(void, RegisterLocalUser, (IRC::UUID uuid), ());
    MOCK_METHOD(void, RegisterServer, (IRC::UUID uuid), ());
    MOCK_METHOD(std::optional<std::shared_ptr<IServer>>, GetServer, (std::string &server_name), ());

};

#endif

