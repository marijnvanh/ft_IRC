#include "Socket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#define UNINITIALIZED -1

TCP::Socket::Socket() : socket_fd_(UNINITIALIZED), address_size_(UNINITIALIZED)
{}

TCP::Socket::~Socket()
{
    if (socket_fd_ != UNINITIALIZED)
        close(socket_fd_);
}

int TCP::Socket::InitSocket(struct addrinfo *addr_info, bool block)
{
    socket_fd_ = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (socket_fd_ == -1)
        return -1; // Error information ?

    /* Set socket to non blocking */
    if (block == true && fcntl(socket_fd_, F_SETFL, O_NONBLOCK) == -1)
    {
        close(socket_fd_);
        socket_fd_ = UNINITIALIZED;
        return -1; // Error information ?
    }

    /* Init Address data */
    address_size_ = addr_info->ai_addrlen;
    memcpy(&address_, addr_info->ai_addr, addr_info->ai_addrlen);
    return socket_fd_;
}

void TCP::Socket::Listen(AddressInfo &address_info, int backlog, bool block)
{
    if (socket_fd_ != UNINITIALIZED)
        throw TCP::Socket::Error("Socket already in use");
    
    struct addrinfo *i = address_info.GetAddrInfo();
    for (;i != NULL; i = i->ai_next)
    {
        if (InitSocket(i, block) == -1)
            continue ;

        /* Turn on REUSEADDR so the server address can be reused in case of a crash. */
        int on = 1;
        if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
        {
            throw TCP::Socket::Error(strerror(errno));
        }

        if (bind(socket_fd_, i->ai_addr, i->ai_addrlen) == -1)
        {
            close(socket_fd_);
            socket_fd_ = UNINITIALIZED;
            continue;
        }
        break ;
    }
    if (i == NULL)
        throw TCP::Socket::Error(strerror(errno));

    if (listen(socket_fd_, backlog) == -1) {
        close(socket_fd_);
        socket_fd_ = UNINITIALIZED;
        throw TCP::Socket::Error(strerror(errno));
    }
}

void TCP::Socket::Connect(AddressInfo &address_info, bool block)
{
    if (socket_fd_ != UNINITIALIZED)
        throw TCP::Socket::Error("Socket already in use");
    
    struct addrinfo *i = address_info.GetAddrInfo();
    for (;i != NULL; i = i->ai_next)
    {
        if (InitSocket(i, block) == -1)
            continue ;
        if (connect(socket_fd_, i->ai_addr, i->ai_addrlen) == -1)
        {
            close(socket_fd_);
            socket_fd_ = UNINITIALIZED;
            continue ; // Error information ?
        }

        break ;
    }
    if (i == NULL)
        throw TCP::Socket::Error(strerror(errno));
}

void TCP::Socket::Accept(int listener_fd)
{
    if (socket_fd_ != UNINITIALIZED)
        throw TCP::Socket::Error("Socket already in use");
    
    address_size_ = sizeof(address_);
    socket_fd_ = accept(listener_fd, (struct sockaddr *)&address_, &address_size_);
    
    if (socket_fd_ == -1)
    {
        /* It is possible that there is nothing to accept due to race conditions */
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            throw TCP::Socket::Error(strerror(errno)); // Should we throw a different error to catch specifically
        else
            throw TCP::Socket::Error(strerror(errno));
    }
}

std::string TCP::Socket::Read()
{
    return "Something";
    // if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
    //     {
    //         // got error or connection closed by client
    //         if (nbytes == 0) {
    //             // connection closed
    //             printf("selectserver: socket %d hung up\n", i);
    //         } else {
    //             perror("recv");
    //         }
    //         close(i); // bye!
    //         FD_CLR(i, &master); // remove from master set
    //     }

}

int TCP::Socket::GetFD()
{
    return socket_fd_;
}

/**
 * @brief Resolve IPv4 or IPv6 sockaddr_in data structure
 * 
 * @param address 
 * @return void* 
 */
static void *GetSockAddrIn(struct sockaddr *address)
{
    if (address->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)address)->sin_addr);
    }
    return &(((struct sockaddr_in6*)address)->sin6_addr);
}

// We could also overload << with all socket related data to string
std::string TCP::Socket::ToStr()
{
    char buffer[INET6_ADDRSTRLEN];
    if (socket_fd_ == UNINITIALIZED)
        return ("Socket not in use");

    inet_ntop(address_.ss_family, GetSockAddrIn((struct sockaddr *)&address_), buffer, sizeof(buffer));
    if (address_.ss_family == AF_INET)
        return (std::string("IPv4 ") + buffer);
    else
        return (std::string("IPv6 ") + buffer);
}
