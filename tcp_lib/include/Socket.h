#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "Resolver.h"

#define DEFAULT_BACKLOG 20
#define BLOCKING true
#define NON_BLOCKING false

namespace TCP
{
    enum SocketState
    {
        kUnInitialized = -1,
        kConnected,
        kDisconnected,
        kError // TODO not sure if we need this
    };

    class Socket
    {
    private:
        int socket_fd_;
        SocketState state_;
        socklen_t address_size_;
        struct sockaddr_storage address_;

        auto InitSocket(struct addrinfo *addr_info, bool block) -> int;
        auto Clear() -> void;

    public:
        Socket();
        ~Socket();

        auto Listen(AddressInfo &address_info,
                    int backlog = DEFAULT_BACKLOG,
                    bool block = NON_BLOCKING) -> void;
        auto Connect(AddressInfo &address_info, bool block = BLOCKING) -> void;
        auto Accept(int listener_fd) -> void;
        auto Recv() -> std::string;
        auto Send(const std::string &data) -> void;

        auto GetFD() const -> int { return socket_fd_; };
        auto GetState() const -> int { return state_; };
        auto ToStr() -> std::string;

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

        Socket (Socket& other) = delete;
        Socket &operator =(Socket& other) = delete;
        Socket (Socket&& other) = delete;
        Socket &operator= (Socket&& other) = delete;
    };
} // namespace TCP

#endif