#ifndef MOCK_USER_H__
#define MOCK_USER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockClient.h"
#include "IUser.h"

class MockUser : public virtual IUser, public MockClient {
    public:
    	MOCK_METHOD(const IRC::UUID&, GetUUID, (), (const, noexcept));
};

#endif