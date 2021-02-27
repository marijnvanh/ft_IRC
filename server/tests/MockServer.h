#ifndef MOCK_SERVER_H__
#define MOCK_SERVER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockClient.h"

class MockServer : public MockClient, public IServer {
    public:
};

#endif