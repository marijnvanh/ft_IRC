#ifndef MOCK_USER_H__
#define MOCK_USER_H__

#include <unordered_map>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockClient.h"
#include "IUser.h"

class MockUser : public virtual IUser, public MockClient {
    public:
    MOCK_METHOD(void, AddChannel, (IChannel*), ());
    MOCK_METHOD(void, RemoveChannel, (const std::string &channel_name), ());
    MOCK_METHOD(void, RemoveUserFromAllChannels, (), ());
    MOCK_METHOD((std::unordered_map<std::string, IChannel*>&), GetChannels, (), ());
    MOCK_METHOD(std::string, GenerateNickMessage, (const std::string &this_server_name), (const));
	MOCK_METHOD(void, SetModeFromString, (std::string const &mode), ());

};

#endif
