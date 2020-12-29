#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "Resolver.h"

#define DEFAULT_BACKLOG 20
#define BLOCKING true
#define NON_BLOCKING false

namespace TCP
{
    class Socket
    {
    private:
        int socket_fd_;
        socklen_t address_size_;
        struct sockaddr_storage address_;

        int InitSocket(struct addrinfo *addr_info, bool block);

    public:
        Socket();
        ~Socket();
        void Listen(AddressInfo &address_info, int backlog = DEFAULT_BACKLOG, bool block = NON_BLOCKING);
        void Connect(AddressInfo &address_info, bool block = BLOCKING);
        void Accept(int listener_fd);
        std::string Recv();
        void Send(const std::string &data) const;
        int GetFD() const;
        std::string ToStr();

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