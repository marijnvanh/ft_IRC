#include "IOController.h"
#include <sys/select.h>
#include <sys/time.h>
#include <memory>

TCP::IOController::IOController(AddressInfo &address_info,
                                std::queue<std::string> &message_queue,
                                int backlog) :
                                message_queue_(message_queue)
{
    listener_.Listen(address_info, backlog);
    FD_ZERO(&master_fd_list_);
    FD_SET(listener_.GetFD(), &master_fd_list_);
    max_fd_ = listener_.GetFD();

    (void)message_queue_;
}

TCP::IOController::~IOController()
{
}

void TCP::IOController::RunOnce(int timeout) // do we need to be more precise aka micro seconds? 
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    read_fds = master_fd_list_;

    struct timeval time_val;
    time_val.tv_sec = timeout;
    time_val.tv_usec = 0; // TODO Fix usec

    int ready_fd_count = select(max_fd_ + 1, &read_fds, NULL, NULL, &time_val);

    if (ready_fd_count == -1) // Look into this
    {
        ;// Do something when select returns -1
    }
    for(int i = 0; i <= max_fd_; i++) // include ready_fd_count
    {
        if (FD_ISSET(i, &read_fds))
        {
            if (i == listener_.GetFD())
                AcceptNewConnection();
            else
                ReadSocket(i);
        }
    }
}

void TCP::IOController::AcceptNewConnection()
{
    auto new_socket = std::make_unique<Socket>();

    try {
        new_socket->Accept(listener_.GetFD());
    }
    catch (TCP::Socket::Error &ex)
    {
        std::cout << ex.what() << std::endl; // What to do with error
        return ; 
    }

    FD_SET(new_socket->GetFD(), &master_fd_list_);
    if (new_socket->GetFD() > max_fd_)
        max_fd_ = new_socket->GetFD();

    sockets_.insert(std::make_pair(new_socket->GetFD(), std::move(new_socket)));
    // How to notify that there's a new connection? Put a string into the queue ?
}

void TCP::IOController::ReadSocket(int socket_fd)
{
    std::cout << sockets_.find(socket_fd)->second->Read();
}