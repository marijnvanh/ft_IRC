#ifndef MOCK_CHANNEL_H__
#define MOCK_CHANNEL_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IChannel.h"

class MockChannel : public virtual IChannel {
    public:

    MOCK_METHOD(void, PushToAll, (std::string irc_message), ());
    MOCK_METHOD(void, PushToLocal, (std::string irc_message), ());
    MOCK_METHOD(void, PushToRemote, (std::string irc_message), ());

    MOCK_METHOD(void, RemoveUser, (std::string irc_message), ());
	MOCK_METHOD(void, AddUser, (IUser* new_user), ());

    MOCK_METHOD(uint32_t, CountUsers, (), ());
};

#endif // MOCK_CHANNEL_H__