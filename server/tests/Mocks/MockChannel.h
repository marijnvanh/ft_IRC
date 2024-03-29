#ifndef MOCK_CHANNEL_H__
#define MOCK_CHANNEL_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IChannel.h"

class MockChannel : public virtual IChannel {
    public:

    MOCK_METHOD(void, PushToLocal, (const std::string &irc_message, std::optional<IRC::UUID> except_uuid), ());

    MOCK_METHOD(bool, RemoveUser, (IRC::UUID uuid), ());
	MOCK_METHOD(void, AddUser, (IUser* new_user, bool is_operator), ());

    MOCK_METHOD(bool, HasUser, (IRC::UUID uuid), ());
    MOCK_METHOD(uint32_t, CountUsers, (), ());
	MOCK_METHOD(const std::string, GetUserListAsString, (char delim), ());

	MOCK_METHOD(bool, AddOperator, (IRC::UUID uuid), ());
	MOCK_METHOD(bool, HasOperator, (IRC::UUID uuid), ());
    MOCK_METHOD(bool, RemoveOperator, (IRC::UUID uuid), ());
};

#endif // MOCK_CHANNEL_H__