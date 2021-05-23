#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <memory>

#include "AddressInfo.h"
#include "ISocket.h"
#include "Logger.h"

#ifdef ENABLE_SSL
    #include "SSL.h"
    #include <openssl/err.h>
#endif

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
        Logger logger;

#ifdef ENABLE_SSL
        SSL* ssl_;
        auto ValidateSSLReturn(int error) -> void;
#endif

        auto InitSocket(struct addrinfo *addr_info, bool block) -> int;
        auto Clear() -> void;
        auto GetSockAddrIn(struct sockaddr *address) const -> void *;

    public:
        Socket(bool enable_ssl);
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