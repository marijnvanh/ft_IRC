#ifndef MOCK_SERVER_H__
#define MOCK_SERVER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockClient.h"
#include "IServer.h"

class MockServer : public IServer, public MockClient {
    public:

    MOCK_METHOD(void, AddClient, (IClient*), ());
    MOCK_METHOD(void, RemoveClient, (IRC::UUID), ());
    MOCK_METHOD(const std::string&, GetServerName, (), (const));
};

#endif
