#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include "AddressInfo.h"
#include "Socket.h"
#include "Message.h"

#include <map>
#include <queue>
#include <memory>

#define DEFAULT_RETRIES 3

namespace TCP
{
    class IOController
    {
    public:
        IOController(AddressInfo &address_info,
                     std::queue<TCP::Message> &recv_message_queue,
                     std::queue<TCP::Message> &send_message_queue,
                     int backlog = DEFAULT_BACKLOG,
                     int max_retries = DEFAULT_RETRIES);
        IOController(std::shared_ptr<Socket> client_socket,
                     std::queue<TCP::Message> &recv_message_queue,
                     std::queue<TCP::Message> &send_message_queue,
                     int max_retries = DEFAULT_RETRIES);
        ~IOController();

        auto RunOnce() -> void;

        class Error : public std::runtime_error
        {
        public:
            Error(const char *msg) : std::runtime_error(msg) {}
        };

        class FailedToSend : public Error
        {
        public:
            FailedToSend(const char *msg) : Error(msg) {}
        };

        class InvalidSocket : public Error
        {
        public:
            InvalidSocket(const char *msg) : Error(msg) {}
        };

    private:
        enum Type { kServer, kClient };

        std::map<int, std::shared_ptr<Socket>> sockets_;
        std::queue<TCP::Message> &recv_message_queue_;
        std::queue<TCP::Message> &send_message_queue_;

        fd_set master_fd_list_;
        int max_fd_;
        int max_retries_;
        TCP::IOController::Type type_;
        std::shared_ptr<Socket> main_socket_;

        auto Init() -> void;
        auto HandleSendQueue(fd_set *write_fds) -> void;
        auto HandleRecvQueue(fd_set *read_fds) -> void;
        auto SendMessage(TCP::Message &message, fd_set *write_fds) -> void;
        auto ReadFromSocket(int socket_fd) -> void;
        auto AcceptNewConnection() -> void;
        auto ValidateSocket(std::shared_ptr<const Socket> socket) -> std::shared_ptr<Socket>;
        auto DeleteSocket(int socket_fd) -> void;
    };
} // namespace TCP

#endif