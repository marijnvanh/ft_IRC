#ifndef __ISOCKET_H__
#define __ISOCKET_H__

#include "AddressInfo.h"

#include <memory>

namespace IRC::TCP
{
    enum SocketState
    {
        kUnInitialized = -1,
        kConnected,
        kDisconnected,
		kReadyToRead
    };

	enum SocketType
	{
		kUnknown = -1,
		kClientSocket = 0,
		kListenerSocket = 1
	};

    class ISocket
    {
    protected:
        int socket_fd_;
		SocketType type_;
        SocketState state_;

    public:
        ISocket() : socket_fd_(kUnInitialized),
            type_(kUnknown),
            state_(kUnInitialized)
        {}
        virtual ~ISocket() {};

        virtual auto Listen(AddressInfo &address_info, int backlog, bool block) -> void = 0;
        virtual auto Connect(AddressInfo &address_info, bool block) -> void = 0;
		virtual auto Close() -> void = 0;
        virtual auto Recv() -> std::string = 0;
        virtual auto Accept(int listener_fd) -> void = 0;
        virtual auto Send(const std::string &data) -> void = 0;

        auto GetFD() const -> int { return socket_fd_; };
        auto SetFD(int socket_fd) -> void { this->socket_fd_ = socket_fd; };
		auto GetType() const -> int { return type_; };
        auto GetState() const -> int { return state_; };
		auto SetState(SocketState state) -> void { this->state_ = state; }

        class Error : public std::runtime_error
        {
        public:
            Error(const char *msg) : std::runtime_error(msg) {}
        };

        class WouldBlock : public Error
        {
            public:
            WouldBlock() : Error("Socket would block") {}
        };

        class Closed : public Error
        {
            public:
            Closed() : Error("Socket closed") {}
        };

        ISocket (ISocket& other) = delete;
        ISocket &operator =(ISocket& other) = delete;
        ISocket (ISocket&& other) = delete;
        ISocket &operator= (ISocket&& other) = delete;
    };
} // namespace IRC::TCP

#endif