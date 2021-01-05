#include "Socket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 512

#ifdef __APPLE__
#define NOSIGPIPE_FLAG SO_NOSIGPIPE
#else
#define NOSIGPIPE_FLAG MSG_NOSIGNAL
#endif

TCP::Socket::Socket() : socket_fd_(kUnInitialized),
                        state_(kUnInitialized),
                        address_size_(kUnInitialized)
{
}

TCP::Socket::~Socket()
{
    if (state_ != kUnInitialized)
        close(socket_fd_);
}

auto TCP::Socket::InitSocket(struct addrinfo *addr_info, bool block) -> int
{
    socket_fd_ = socket(addr_info->ai_family, addr_info->ai_socktype, addr_info->ai_protocol);
    if (socket_fd_ == -1)
        return -1;

    if (block == false && fcntl(socket_fd_, F_SETFL, O_NONBLOCK) == -1)
    {
        Clear();
        return -1;
    }
    address_size_ = addr_info->ai_addrlen;
    memcpy(&address_, addr_info->ai_addr, addr_info->ai_addrlen);
    return socket_fd_;
}

auto TCP::Socket::Clear() -> void
{
    close(socket_fd_);
    socket_fd_ = kUnInitialized;
    address_size_ = kUnInitialized;
    state_ = kUnInitialized;
}

/**
 * @brief Create a listening socket
 * 
 * @param address_info 
 * @param backlog pending connection queue size, default is 20
 * @param block set to true for non-blocking socket
 * 
 * @exception Socket::Error
 */
auto TCP::Socket::Listen(AddressInfo &address_info, int backlog, bool block) -> void
{
    if (state_ != kUnInitialized)
        throw TCP::Socket::Error("Socket already in use");

    struct addrinfo *i = address_info.GetAddrInfo();
    for (; i != NULL; i = i->ai_next)
    {
        if (InitSocket(i, block) == -1)
            continue;

        /* Turn on REUSEADDR so the server address can be reused in case of a crash. */
        int on = 1;
        if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
        {
            Clear();
            throw TCP::Socket::Error(strerror(errno));
        }

        if (bind(socket_fd_, i->ai_addr, i->ai_addrlen) == -1)
        {
            Clear();
            continue;
        }
        break;
    }
    if (i == NULL)
        throw TCP::Socket::Error(strerror(errno));

    if (listen(socket_fd_, backlog) == -1)
    {
        Clear();
        throw TCP::Socket::Error(strerror(errno));
    }
    state_ = kConnected;
}

/**
 * @brief Create a basic connection socket
 * 
 * @param address_info 
 * @param block set to true for non-blocking socket
 * 
 * @exception Socket::Error
 */
auto TCP::Socket::Connect(AddressInfo &address_info, bool block) -> void
{
    if (state_ != kUnInitialized)
        throw TCP::Socket::Error("Socket already in use");

    struct addrinfo *i = address_info.GetAddrInfo();
    for (; i != NULL; i = i->ai_next)
    {
        if (InitSocket(i, block) == -1)
            continue;
        if (connect(socket_fd_, i->ai_addr, i->ai_addrlen) == -1)
        {
            Clear();
            continue;
        }
        break;
    }
    if (i == NULL)
        throw TCP::Socket::Error(strerror(errno));
    state_ = kConnected;
}

/**
 * @brief Accept a new connection
 * 
 * @param listener_fd file descriptor of an active listener socket
 * 
 * @exception Socket::WouldBlock
 * @exception Socket::Error
 */
auto TCP::Socket::Accept(int listener_fd) -> void
{
    if (state_ != kUnInitialized)
        throw TCP::Socket::Error("Socket already in use");

    address_size_ = sizeof(address_);
    socket_fd_ = accept(listener_fd, (struct sockaddr *)&address_, &address_size_);

    if (socket_fd_ == -1)
    {
        address_size_ = kUnInitialized;
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            throw TCP::Socket::WouldBlock();
        else
            throw TCP::Socket::Error(strerror(errno));
    }
    state_ = kConnected;
}

/**
 * @brief Start reading from a socket
 * Read will throw Socket::Closed when it detects a closed connection
 * 
 * @return std::string
 * 
 * @exception Socket::Closed
 * @exception Socket::WouldBlock only on non-blocking sockets
 * @exception Socket::Error
 */
auto TCP::Socket::Recv() -> std::string
{
    char buffer[BUFFER_SIZE];

    int received_bytes = recv(socket_fd_, buffer, BUFFER_SIZE - 1, 0);
    if (received_bytes == 0)
    {
        state_ = kDisconnected;
        throw TCP::Socket::Closed();
    }
    if (received_bytes == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            throw TCP::Socket::WouldBlock();
        else
        {
            state_ = kDisconnected;
            throw TCP::Socket::Error(strerror(errno));
        }
    }
    buffer[received_bytes] = '\0';
    return std::string(buffer, received_bytes);
}

/**
 * @brief Send data over a socket
 * Send will throw Socket::Closed when it detects a closed connection
 * 
 * @param data
 * 
 * @exception Socket::Closed
 * @exception Socket::WouldBlock only on non-blocking sockets
 * @exception Socket::Error
 */
auto TCP::Socket::Send(const std::string &data) -> void
{
    size_t data_size = data.size();
    size_t bytesleft = data_size;
    size_t total_send = 0;
    const char *raw_data = data.c_str();
    while (total_send < data_size)
    {
        /* Set NOSIGPIPE_FLAG to make sure that send doesn't send a signal on lost connection */
        int send_bytes = send(socket_fd_, &raw_data[total_send], bytesleft, NOSIGPIPE_FLAG);
        if (send_bytes == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                throw TCP::Socket::WouldBlock();

            /* Normally EPIPE gets set on a lost connection but Mac sometimes returns EPROTOTYPE */
            else if (errno == EPIPE || errno == EPROTOTYPE)
            {
                state_ = kDisconnected;
                throw TCP::Socket::Closed();
            }
            else
            {
                state_ = kDisconnected;
                throw TCP::Socket::Error(strerror(errno));
            }
        }

        total_send += send_bytes;
        bytesleft -= send_bytes;
    }
}

/**
 * @brief Resolve IPv4 or IPv6 sockaddr_in data structure
 * 
 * @param address 
 * @return void* 
 */
auto TCP::Socket::GetSockAddrIn(struct sockaddr *address) const -> void *
{
    if (address->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)address)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)address)->sin6_addr);
}

auto TCP::operator<<(std::ostream& os, const TCP::Socket& socket) -> std::ostream &
{
    char buffer[INET6_ADDRSTRLEN];
    if (socket.GetState() == TCP::SocketState::kUnInitialized)
    {
        os << "Socket not in use";
        return os;
    }
    inet_ntop(socket.address_.ss_family, socket.GetSockAddrIn((struct sockaddr *)&socket.address_), buffer, sizeof(buffer));
    if (socket.address_.ss_family == AF_INET)
        os << "IPv4 " << buffer;
    else
        os << "IPv6 " << buffer;
        
    return os;
}
