#include "IOController.h"
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <memory>

TCP::IOController::IOController(AddressInfo &address_info,
                                std::queue<TCP::Message> &recv_message_queue,
                                std::queue<TCP::Message> &send_message_queue,
                                int backlog) :
                                recv_message_queue_(recv_message_queue),
                                send_message_queue_(send_message_queue)
{
    listener_.Listen(address_info, backlog);
    FD_ZERO(&master_fd_list_);
    FD_SET(listener_.GetFD(), &master_fd_list_);
    max_fd_ = listener_.GetFD();
}

TCP::IOController::~IOController() {}

/**
 * @brief Accepts new connections and sends/receives messages
 * 
 * @exception TCP::IOController::Error when select fails
 * @param timeout in seconds
 */
void TCP::IOController::RunOnce(int timeout) //TODO do we need to be more precise aka micro seconds? 
{
    struct timeval time_val;
    
    time_val.tv_sec = 0;
    time_val.tv_usec = 0;
    HandleSendQueue(&time_val);
    
    time_val.tv_sec = timeout;
    time_val.tv_usec = 0; // TODO Fix usec
    HandleRecvQueue(&time_val);
}

void TCP::IOController::HandleSendQueue(struct timeval *time_val)
{
    fd_set write_fds;
    FD_ZERO(&write_fds);
    write_fds = master_fd_list_;

    int ready_write_fds = select(max_fd_ + 1, NULL, &write_fds, NULL, time_val);

    if (ready_write_fds == -1)
        throw TCP::IOController::Error(strerror(errno));
    
    while (send_message_queue_.empty() == false)
    { 
        auto message = send_message_queue_.front();
        const Socket &socket = message.GetSocket();
        if (FD_ISSET(socket.GetFD(), &write_fds))
            SendMessage(socket, message.GetData());
        else
        {
            ; //TODO store msg for later? retry count
        }
        send_message_queue_.pop();
    };
}

void TCP::IOController::SendMessage(const Socket &socket, std::string &data)
{
    try {
        socket.Send(data);
    }
    catch (TCP::Socket::Closed &ex)
    {
        //TODO remove socket safely (remove references in code) from list
        FD_CLR(socket.GetFD(), &master_fd_list_); //TODO How to lower max_fd?
        sockets_.erase(socket.GetFD());
        //TODO How to notify that there's a closed connection? Put a string into the queue ?
    }
    catch (TCP::Socket::WouldBlock &ex)
    {
        ; //TODO Keep message in queue to send later ?
    }
    catch (TCP::Socket::Error &ex)
    {
        std::cerr << "Could not send message: " << ex.what() << std::endl;
        ; //TODO how to handle, retry, kill connection ?
    }
}

void TCP::IOController::HandleRecvQueue(struct timeval *time_val)
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    read_fds = master_fd_list_;

    int total_ready_fds = select(max_fd_ + 1, &read_fds, NULL, NULL, time_val);

    if (total_ready_fds == -1)
        throw TCP::IOController::Error(strerror(errno));

    int i = 0;
    int fd_read_count = 0;
    while (i <= max_fd_ && fd_read_count < total_ready_fds)
    {
        if (FD_ISSET(i, &read_fds))
        {
            if (i == listener_.GetFD())
                AcceptNewConnection();
            else
                ReadSocket(i);
            fd_read_count++;
        }
        i++;
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
        std::cerr << "Could not accept new connection: " << ex.what() << std::endl;
        return ; 
    }

    FD_SET(new_socket->GetFD(), &master_fd_list_);
    if (new_socket->GetFD() > max_fd_)
        max_fd_ = new_socket->GetFD();

    sockets_.insert(std::make_pair(new_socket->GetFD(), std::move(new_socket)));
    //TODO How to notify that there's a new connection? Put a string into the queue ?
}

void TCP::IOController::ReadSocket(int socket_fd)
{
    try {
        auto socket = sockets_.find(socket_fd);
        std::string data = socket->second->Recv();
        recv_message_queue_.push(TCP::Message(*(socket->second), std::move(data)));
    }
    catch (TCP::Socket::Closed &ex)
    {
        FD_CLR(socket_fd, &master_fd_list_); //TODO How to lower max_fd?
        sockets_.erase(socket_fd);
        //TODO How to notify that there's a closed connection? Put a string into the queue ?
    }
    catch (TCP::Socket::WouldBlock &ex)
    {
        ; //TODO Do something when it blocks
    }
    catch (TCP::Socket::Error &ex)
    {
        ; //TODO What to do if socket errors, close connection?
        FD_CLR(socket_fd, &master_fd_list_); //TODO How to lower max_fd?
        sockets_.erase(socket_fd);
    }
}
