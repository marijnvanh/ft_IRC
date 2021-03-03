#ifndef MOCK_SERVER_H__
#define MOCK_SERVER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockClient.h"
#include "IServer.h"

class MockServer : public IServer, public MockClient {
    public:

    MOCK_METHOD(void, AddClient, (std::shared_ptr<IClient>), ());
    MOCK_METHOD(void, RemoveClient, (IRC::UUID), ());
};

#endif