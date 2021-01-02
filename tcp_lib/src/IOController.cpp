#include "IOController.h"
#include <sys/select.h>
#include <sys/time.h>
#include <string.h>
#include <memory>
/**
 * @brief Construct a new TCP::IOController::IOController object
 * 
 * @param address_info address info for the listener socket
 * @param recv_message_queue incomming msg queue
 * @param send_message_queue outgoing msg queue
 * @param backlog amount of pending connections
 * @param max_retries maximum amount of send retries
 */
TCP::IOController::IOController(AddressInfo &address_info,
                                std::queue<TCP::Message> &recv_message_queue,
                                std::queue<TCP::Message> &send_message_queue,
                                int backlog,
                                int max_retries) :
                                recv_message_queue_(recv_message_queue),
                                send_message_queue_(send_message_queue),
                                max_retries_(max_retries)
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
auto TCP::IOController::RunOnce(int timeout) -> void //TODO do we need to be more precise aka micro seconds? 
{
    struct timeval time_val;
    time_val.tv_sec = timeout;
    time_val.tv_usec = 0; // TODO Fix usec
    
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

    HandleSendQueue(&write_fds);
    HandleRecvQueue(&read_fds);
}

auto TCP::IOController::HandleSendQueue(fd_set *write_fds) -> void
{
    for (size_t message_count = send_message_queue_.size(); message_count > 0; message_count--)
    {
        auto message = send_message_queue_.front();
        try {
            SendMessage(message, write_fds);
        }
        catch (TCP::IOController::FailedToSend &ex)
        {
            std::cerr << "Failed to send message: " << ex.what();
            std::cerr << "Retries: " << message.GetRetries() << std::endl;
            if (message.GetRetries() < max_retries_)
            {
                message.Retry();
                send_message_queue_.push(std::move(message));//TODO I dont udnerstand move, do I move here?
            }
        }
        send_message_queue_.pop();
    };
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
            socket->Send(message.GetData());
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
        DeleteSocket(message.GetFD()); // TODO How to handle error
        throw TCP::IOController::FailedToSend(ex.what());
    }
}

auto TCP::IOController::HandleRecvQueue(fd_set *read_fds) -> void
{
    int i = 0;
    while (i <= max_fd_)
    {
        if (FD_ISSET(i, read_fds))
        {
            if (i == listener_.GetFD())
                AcceptNewConnection();
            else
                ReadFromSocket(i);
        }
        i++;
    }
}

auto TCP::IOController::AcceptNewConnection() -> void
{
    auto new_socket = std::make_shared<Socket>();

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

    sockets_.insert(std::make_pair(new_socket->GetFD(), new_socket));

    recv_message_queue_.push(TCP::Message(new_socket, ""));
}

auto TCP::IOController::ReadFromSocket(int socket_fd) -> void
{
    auto socket = sockets_.find(socket_fd);
    try {
        std::string data = socket->second->Recv();
        recv_message_queue_.push(TCP::Message(socket->second, std::move(data)));
    }
    catch (TCP::Socket::Closed &ex)
    {
        std::cerr << "Read failed: " << ex.what() << std::endl;
        DeleteSocket(socket_fd);
    }
    catch (TCP::Socket::WouldBlock &ex)
    {
        std::cerr << "Read failed: " << ex.what() << std::endl;
        ; //TODO Do something when it blocks or just ignore?
    }
    catch (TCP::Socket::Error &ex)
    {
        DeleteSocket(socket_fd); //TODO do we close connection on error?
        std::cerr << "Read failed: " << ex.what() << std::endl;
    }
}

auto TCP::IOController::DeleteSocket(int socket_fd) -> void
{
    /* Notify user with an empty message */
    auto socket = sockets_.find(socket_fd);
    recv_message_queue_.push(TCP::Message(socket->second, ""));

    FD_CLR(socket_fd, &master_fd_list_); //TODO How to lower max_fd?
    sockets_.erase(socket_fd);    
}

//TODO Fix socket close