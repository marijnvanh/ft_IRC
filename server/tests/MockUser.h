#ifndef MOCK_USER_H__
#define MOCK_USER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockClient.h"
#include "IUser.h"

class MockUser : public IUser, public MockClient {
    public:
};

#endif