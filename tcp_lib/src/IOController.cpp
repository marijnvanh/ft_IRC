#include "IOController.h"
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <memory>

/**
 * @brief Construct a new TCP::IOController object
 * 
 * @param max_retries maximum amount of send retries
 */
TCP::IOController::IOController(int max_retries) :
                                max_retries_(max_retries)
{
	max_fd_ = 0;
    FD_ZERO(&master_fd_list_);
}

TCP::IOController::~IOController() {}

/**
 * @brief Accepts new connections and sends/receives messages
 * 
 * @exception TCP::IOController::Error when select fails
 * @exception TCP::IOController::Error when main socket closes
 */
auto TCP::IOController::RunOnce() -> void
{
    /* Set time_val to 0 so select doesn't block */
    struct timeval time_val = {0, 0};
    
    fd_set read_fds;
    fd_set write_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    read_fds = master_fd_list_;
    write_fds = master_fd_list_;

    int total_ready_fds = select(max_fd_ + 1, &read_fds, &write_fds, NULL, &time_val);

	if (total_ready_fds == -1)
		throw TCP::IOController::Error(strerror(errno));

	if (total_ready_fds == 0)
		return ;

	for (int i = 0; i <= max_fd_; i++)
	{
		if (FD_ISSET(i, &read_fds))
		{
			auto socket = sockets_.find(i);
			socket->second->SetState(TCP::SocketState::kReadyToRead);
		}
	}
}

/**
 * @brief Validate that a socket is still ok to use and return a mutable reference
 * 
 * @param socket 
 * @return Socket& 
 */
auto TCP::IOController::ValidateSocket(std::shared_ptr<const Socket> socket) -> std::shared_ptr<Socket>
{
    int socket_fd = socket->GetFD();

    if (FD_ISSET(socket_fd, &master_fd_list_) == false)
        throw TCP::IOController::InvalidSocket("fd unknown");    

    auto mutable_socket = sockets_.find(socket_fd);
    if (mutable_socket->second == socket)
        return mutable_socket->second;
    else
        throw TCP::IOController::InvalidSocket("fd unknown");
}

auto TCP::IOController::SendMessage(TCP::Message &message, fd_set *write_fds) -> void
{
    try {
        std::shared_ptr<Socket> socket = ValidateSocket(message.GetSocket());
        if (FD_ISSET(message.GetFD(), write_fds))
            socket->Send(*message.GetData());
        else
            throw TCP::IOController::FailedToSend("Socket not ready for writing");
    }
    catch (TCP::IOController::InvalidSocket &ex)
    {
        throw TCP::IOController::FailedToSend(ex.what());
    }
    catch (TCP::Socket::Closed &ex)
    {
        DeleteSocket(message.GetFD());
        throw TCP::IOController::FailedToSend(ex.what());
    }
    catch (TCP::Socket::WouldBlock &ex)
    {
        throw TCP::IOController::FailedToSend(ex.what());
    }
    catch (TCP::Socket::Error &ex)
    {
        DeleteSocket(message.GetFD());
        throw TCP::IOController::FailedToSend(ex.what());
    }
}

auto TCP::IOController::AcceptNewConnections(void (*f)(std::shared_ptr<Socket>)) -> void
{
	// TODO: Rob - As it stands, this loop accepts one client for each listener socket during every IOController revolution.
	// Might be better to rework this to keep accepting new connections until there's none left ( while(accept) ).
	for (int i = 0; i <= max_fd_; i++)
	{
   		auto listener_socket = sockets_.find(i);
		
		// This if statement is blown up a little bit, might be able to shrink it down by applying different check(s)?
		if (listener_socket == sockets_.end() ||
			listener_socket->second->GetType() != TCP::SocketType::kListenerSocket ||
			listener_socket->second->GetState() != TCP::SocketState::kReadyToRead)
			continue ;

		try
		{
			auto new_socket = listener_socket->second->Accept();
		
			this->AddSocket(new_socket);
			new_socket->SetState(TCP::SocketState::kConnected);
			new_socket->SetType(TCP::SocketType::kClientSocket);

			f(new_socket);

			listener_socket->second->SetState(TCP::SocketState::kConnected);
		}
   		catch (TCP::Socket::Error &ex)
		{
			std::cerr << "Could not accept new connection: " << ex.what() << std::endl;
			return ; 
		}
	}
}

auto TCP::IOController::AddSocket(std::shared_ptr<Socket> socket) -> void
{
	if (socket->GetFD() > max_fd_)
	{
		max_fd_ = socket->GetFD();
	}

	FD_SET(socket->GetFD(), &master_fd_list_);
	sockets_.insert(std::make_pair(socket->GetFD(), socket));
}

auto TCP::IOController::DeleteSocket(int socket_fd) -> void
{
    auto socket = sockets_.find(socket_fd);

	if (socket == sockets_.end())
	{
		return ;
	}

    FD_CLR(socket_fd, &master_fd_list_);
	socket->second->SetState(TCP::SocketState::kDisconnected);

    /* Lower max_fd when we delete the previous highest fd */
    if (socket_fd == max_fd_)
    {
        while (FD_ISSET(max_fd_, &master_fd_list_) == false && max_fd_ > 0)
		{
            max_fd_--;
		}
    }
    sockets_.erase(socket_fd);    
}
