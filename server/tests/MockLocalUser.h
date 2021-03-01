#ifndef MOCK_LOCALUSER_H__
#define MOCK_LOCALUSER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockUser.h"
#include "ILocalUser.h"

class MockLocalUser : public ILocalUser, public MockUser {
    public:
};

#endif