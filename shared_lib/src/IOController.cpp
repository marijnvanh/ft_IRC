#include "IOController.h"
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <memory>
#include <vector>

using namespace IRC;

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

	this->UpdateSocketStates(&read_fds);
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

auto TCP::IOController::AcceptNewConnections(const std::function<void(std::shared_ptr<Socket>)>& newSocketCallback) -> void
{
	// TODO: Rob - As it stands, this loop accepts one client for each listener socket during every IOController revolution.
	// Might be better to rework this to keep accepting new connections until there's none left ( while(accept) ).
	for (auto const &listener_socket:sockets_)
	{
		if (listener_socket.second->GetType() == TCP::SocketType::kListenerSocket &&
			listener_socket.second->GetState() == TCP::SocketState::kReadyToRead)
		{
			listener_socket.second->SetState(SocketState::kConnected);
			try
			{
				auto new_socket = std::make_shared<Socket>();
				new_socket->Accept(listener_socket.second->GetFD());
			
				this->AddSocket(new_socket);

				f(new_socket);
			}
			catch (TCP::Socket::Error &ex)
			{
				std::cerr << "Could not accept new connection: " << ex.what() << std::endl;
				return ; 
			}
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

auto TCP::IOController::UpdateSocketStates(fd_set *ready_fds) -> void
{
	for (auto it = sockets_.cbegin(); it != sockets_.cend();)
	{
		if (it->second->GetState() == TCP::SocketState::kDisconnected)
		{
   			FD_CLR(it->second->GetFD(), &master_fd_list_);
			
			if (it->second->GetFD() == max_fd_)
			{
				while (FD_ISSET(max_fd_, &master_fd_list_) == false && max_fd_ > 0)
				{
					max_fd_--;
				}
			}

			it = sockets_.erase(it);
		}
		else
		{
			if (FD_ISSET(it->second->GetFD(), ready_fds))
			{
				it->second->SetState(TCP::SocketState::kReadyToRead);
			}			

			++it;
		}		
	}
}
