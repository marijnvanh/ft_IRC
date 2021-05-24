#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <memory>
#include <vector>

#include "IOController.h"
#include "Socket.h"

using namespace IRC;

TCP::IOController::IOController() :
	logger("IOController")
{
	max_fd_ = 0;
    FD_ZERO(&master_fd_list_);
}

TCP::IOController::~IOController() {}

auto TCP::IOController::RunOnce() -> void
{
    /* Set time_val to 0 so select doesn't block */
    struct timeval time_val = {0, 0};
    
    fd_set read_fds;
    fd_set write_fds;

	ValidateSockets();

	if (sockets_.size() == 0)
		return ;

	// This needs to happen AFTER validation, because sockets CAN BE REMOVED during said validation.
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    read_fds = master_fd_list_;
    write_fds = master_fd_list_;

    int total_ready_fds = select(max_fd_ + 1, &read_fds, &write_fds, NULL, &time_val);

	if (total_ready_fds == -1)
	{
		throw TCP::IOController::Error(strerror(errno));
	}

	this->UpdateSocketStates(&read_fds);
}

auto TCP::IOController::AcceptNewConnections(const std::function<void(std::shared_ptr<ISocket>)>& newSocketCallback) -> void
{
	for (auto const &listener_socket:sockets_)
	{
		if (listener_socket.second->GetType() == TCP::SocketType::kListenerSocket &&
			listener_socket.second->GetState() == TCP::SocketState::kReadyToRead)
		{
			listener_socket.second->SetState(SocketState::kConnected);
			try
			{
				bool enable_ssl = false;
#ifdef ENABLE_SSL
				if (listener_socket.second->IsSSLEnabled())
					enable_ssl = true;
				logger.Log(LogLevel::DEBUG, "Accept new connect with SSL Enabled");
#endif			
				auto new_socket = std::make_shared<Socket>(enable_ssl);
				new_socket->Accept(listener_socket.second->GetFD());
			
				this->AddSocket(new_socket);

				newSocketCallback(new_socket);
			}
			catch (TCP::ISocket::Error &ex)
			{
				logger.Log(LogLevel::WARNING, "Could not accept new connection: %s", ex.what());
				return ; 
			}
		}
	}
}

auto TCP::IOController::AddSocket(std::shared_ptr<ISocket> socket) -> void
{
	if (socket->GetFD() > max_fd_)
	{
		max_fd_ = socket->GetFD();
	}

	FD_SET(socket->GetFD(), &master_fd_list_);
	sockets_.insert(std::make_pair(socket->GetFD(), socket));
}

auto TCP::IOController::RemoveSocket(std::shared_ptr<ISocket> socket) -> void
{
	FD_CLR(socket->GetFD(), &master_fd_list_);

	if (socket->GetFD() == max_fd_)
	{
		while (FD_ISSET(max_fd_, &master_fd_list_) == false && max_fd_ > 0)
		{
			max_fd_--;
		}
	}

	sockets_.erase(socket->GetFD());
}

auto TCP::IOController::ValidateSockets() -> void
{
	for (auto it = sockets_.cbegin(), next_it = it; it != sockets_.cend(); it = next_it)
	{
		++next_it;
		if (it->second->GetState() == TCP::SocketState::kDisconnected)
		{
			RemoveSocket(it->second);
		}
	}
}

auto TCP::IOController::UpdateSocketStates(fd_set *ready_fds) -> void
{
	for (auto it = sockets_.begin(), next_it = it; it != sockets_.end(); it = next_it)
	{
		++next_it;
		if (it->second->GetState() != TCP::SocketState::kDisconnected)
		{
			if (FD_ISSET(it->second->GetFD(), ready_fds))
			{
				it->second->SetState(TCP::SocketState::kReadyToRead);
			}
		}		
	}
}
