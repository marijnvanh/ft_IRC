#ifndef MOCK_SOCKET_H__
#define MOCK_SOCKET_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ISocket.h"

class MockSocket : public virtual IRC::TCP::ISocket {
    public:
    MOCK_METHOD(void, Listen, (IRC::TCP::AddressInfo &address_info, int backlog, bool block), ());
    MOCK_METHOD(void, Connect, (IRC::TCP::AddressInfo &address_info, bool block), ());
	MOCK_METHOD(void, Close, (), ());
    MOCK_METHOD(std::string, Recv, (), ());
    MOCK_METHOD(void, Accept, (int listener_fd), ());
    MOCK_METHOD(void, Send, (const std::string &data), ());
};

#endif // MOCK_SOCKET_H__