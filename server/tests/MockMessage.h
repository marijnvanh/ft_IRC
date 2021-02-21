#ifndef MOCK_MESSAGE_H__
#define MOCK_MESSAGE_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IMessage.h"

class MockMessage : public IMessage {
    public:

    MOCK_METHOD(const IRC::UUID, GetUUID, (), (const, noexcept));
    MOCK_METHOD(const std::string &, GetCommand, (), (const, noexcept));
    MOCK_METHOD(const std::vector<std::string> &, GetParams, (), (const, noexcept));
    MOCK_METHOD(const std::optional<std::string>, GetServername, (), (const, noexcept));
    MOCK_METHOD(const std::optional<std::string>, GetNickname, (), (const, noexcept));
};

#endif