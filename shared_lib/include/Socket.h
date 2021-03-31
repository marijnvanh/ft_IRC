#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "AddressInfo.h"
#include "ISocket.h"

#include <memory>

#define DEFAULT_BACKLOG 20
#define BLOCKING true
#define NON_BLOCKING false

namespace IRC::TCP
{
    class Socket : public ISocket
    {
    private:
        socklen_t address_size_;
        struct sockaddr_storage address_;

        auto InitSocket(struct addrinfo *addr_info, bool block) -> int;
        auto Clear() -> void;
        auto GetSockAddrIn(struct sockaddr *address) const -> void *;

    public:
        Socket();
        ~Socket();

        auto Listen(AddressInfo &address_info,
                    int backlog = DEFAULT_BACKLOG,
                    bool block = NON_BLOCKING) -> void override;
        auto Connect(AddressInfo &address_info, bool block = BLOCKING) -> void override;
		auto Close() -> void override;
        auto Recv() -> std::string override;
        auto Accept(int listener_fd) -> void override;
        auto Send(const std::string &data) -> void override;

        friend auto operator<<(std::ostream& os, const Socket& socket) -> std::ostream &;

        Socket (Socket& other) = delete;
        Socket &operator =(Socket& other) = delete;
        Socket (Socket&& other) = delete;
        Socket &operator= (Socket&& other) = delete;
    };

    auto operator<<(std::ostream& os, const Socket& socket) -> std::ostream &;
} // namespace IRC::TCP

#endif