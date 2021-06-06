#ifndef MOCK_CLIENT_H__
#define MOCK_CLIENT_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IClient.h"

class MockClient : public virtual IClient {
    public:

    MOCK_METHOD1(Push, void(std::string irc_message));
    MOCK_METHOD0(Receive, std::optional<std::string>());
    MOCK_METHOD0(SendAll, void());
    MOCK_CONST_METHOD0(GetUUID, const IRC::UUID&());
    MOCK_METHOD(IServer*, GetLocalServer, (), (const));
    MOCK_METHOD(IServer*, GetRemoteServer, (), (const));
};

#endif
