#ifndef MOCK_IOHANDLER_H__
#define MOCK_IOHANDLER_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IIOHandler.h"

class MockIOHandler : public IRC::IIOHandler {
    public:

    MOCK_METHOD1(Send, void(std::string msg));
    MOCK_METHOD0(Receive, std::optional<std::string>());
};

#endif