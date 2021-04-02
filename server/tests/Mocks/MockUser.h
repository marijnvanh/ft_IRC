#ifndef MOCK_USER_H__
#define MOCK_USER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockClient.h"
#include "IUser.h"

class MockUser : public virtual IUser, public MockClient {
    public:
    MOCK_METHOD(void, AddChannel, (IChannel*), ());
    MOCK_METHOD(void, RemoveChannel, (const std::string &channel_name), ());
    MOCK_METHOD(void, RemoveUserFromAllChannels, (), ());
};

#endif