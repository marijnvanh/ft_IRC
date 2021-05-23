#include "Socket.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace IRC;

#define BUFFER_SIZE 512

#ifdef __APPLE__
#define NOSIGPIPE_FLAG SO_NOSIGPIPE
#else
#define NOSIGPIPE_FLAG MSG_NOSIGNAL
#endif

TCP::Socket::Socket(bool enable_ssl) : address_size_(kUnInitialized),
    logger("SOCKET")
{
#ifdef ENABLE_SSL
    ssl_enabled_ = enable_ssl;
    if (ssl_enabled_) {
        logger.Log(LogLevel::DEBUG, "Created socket with SSL enabled", enable_ssl);
        if (ctx_g == NULL) {
            throw TCP::Socket::Error("SSL not initialized");
        }
    }
    else
        logger.Log(LogLevel::DEBUG, "Created socket with SSL disabled", enable_ssl);
    ssl_ = NULL;
#else
    logger.Log(LogLevel::DEBUG, "Created socket", enable_ssl);
    ssl_enabled_ = false;
#endif

}

TCP::Socket::~Socket()
{
	this->Close();
#ifdef ENABLE_SSL
    if (ssl_ != NULL)
        SSL_free (ssl_);
#endif
	socket_fd_ = -1;
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
	type_ = kListenerSocket;
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
	type_ = kClientSocket;
}

auto TCP::Socket::Close() -> void
{
	if (this->state_ == SocketState::kUnInitialized || this->state_ == SocketState::kDisconnected)
	{
		return ;
	}

	close(this->socket_fd_);

	this->SetState(SocketState::kDisconnected);
}

/**
 * @brief Accept a new connection
 * 
 * @exception Socket::Error
 * @exception Socket::WouldBlock
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

#ifdef ENABLE_SSL
    if (ssl_enabled_) {
        logger.Log(LogLevel::DEBUG, "Set SSL on socket %d", socket_fd_);
        ssl_ = SSL_new (ctx_g);
        if (ssl_ == NULL) {
            ERR_print_errors_fp(stderr);
            throw TCP::Socket::Error("SSLError");
        }
        SSL_set_fd(ssl_, socket_fd_);
        int ret = SSL_accept(ssl_);
        logger.Log(LogLevel::DEBUG, "SSL_accept return %d", ret);
        try {
            ValidateSSLReturn(ret);
        }
        catch (TCP::ISocket::WouldBlock &ex) {
            ;
        }
    }
    else
        logger.Log(LogLevel::DEBUG, "Created socket without SSL");
#endif

    state_ = kConnected;
	type_ = kClientSocket;
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
    int received_bytes = 0;
	state_ = TCP::SocketState::kConnected;

    if (ssl_enabled_) {
#ifdef ENABLE_SSL
        received_bytes = SSL_read(ssl_, buffer, BUFFER_SIZE - 1);
        logger.Log(LogLevel::DEBUG, "SSL_read received bytes: %d", received_bytes);
        ValidateSSLReturn(received_bytes);
#endif
    }
    else {   
        received_bytes = recv(socket_fd_, buffer, BUFFER_SIZE - 1, 0);
        if (received_bytes == 0)
        {
            this->Close();
            throw TCP::Socket::Closed();
        }
        if (received_bytes == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                throw TCP::Socket::WouldBlock();
            else
            {
                this->Close();
                throw TCP::Socket::Error(strerror(errno));
            }
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
    int send_bytes = 0;
    while (total_send < data_size)
    {
        if (ssl_enabled_) {
#ifdef ENABLE_SSL
            send_bytes = SSL_write(ssl_, &raw_data[total_send], bytesleft);
            logger.Log(LogLevel::DEBUG, "SSL_write send bytes: %d", send_bytes);
            ValidateSSLReturn(send_bytes);
#endif 
        }
        else {
            /* Set NOSIGPIPE_FLAG to make sure that send doesn't send a signal on lost connection */
            send_bytes = send(socket_fd_, &raw_data[total_send], bytesleft, NOSIGPIPE_FLAG);
            if (send_bytes == -1)
            {            
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    throw TCP::Socket::WouldBlock();

                /* Close the socket on send error */
                this->Close();

                /* Normally EPIPE gets set on a lost connection but Mac sometimes returns EPROTOTYPE */
                if (errno == EPIPE || errno == EPROTOTYPE)
                    throw TCP::Socket::Closed();

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

#ifdef ENABLE_SSL
auto TCP::Socket::ValidateSSLReturn(int error) -> void
{
    if (error > 0){
        return;
    }

    error = SSL_get_error(ssl_, error);
    if (error == SSL_ERROR_WANT_READ)
    {
        logger.Log(LogLevel::DEBUG, "SSL error want read (would block)");
        throw TCP::Socket::WouldBlock();
    }
    else if (error == SSL_ERROR_WANT_WRITE)
    {
        logger.Log(LogLevel::DEBUG, "SSL want write (would block)");
        throw TCP::Socket::WouldBlock();
    }
    else if (error == SSL_ERROR_SYSCALL || error == SSL_ERROR_SSL)
    {
        this->Close();
        logger.Log(LogLevel::WARNING, "SSL connection closed with an error");
        ERR_print_errors_fp(stderr);
        throw TCP::Socket::Error(strerror(errno));
    }
    else if (error == SSL_ERROR_ZERO_RETURN)
    {
        this->Close();
        logger.Log(LogLevel::WARNING, "SSL connection closed");
        throw TCP::Socket::Closed();
    }
}
#endif