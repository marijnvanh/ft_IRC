#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include "Resolver.h"
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
    private:

        std::map<int, std::shared_ptr<Socket>> sockets_;
        std::queue<TCP::Message> &recv_message_queue_;
        std::queue<TCP::Message> &send_message_queue_;

        fd_set master_fd_list_;
        int max_fd_;
        int max_retries_;
        Socket listener_;

        void HandleSendQueue(struct timeval *time_val);
        void HandleRecvQueue(struct timeval *time_val);
        void SendMessage(int socket_fd, std::string &data);
        void ReadSocket(int socket_fd);
        void AcceptNewConnection();


    public:
        IOController(AddressInfo &address_info,
                     std::queue<TCP::Message> &recv_message_queue,
                     std::queue<TCP::Message> &send_message_queue,
                     int backlog = DEFAULT_BACKLOG,
                     int max_retries = DEFAULT_RETRIES);
        ~IOController();
        void RunOnce(int timeout);

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

    };
} // namespace TCP

#endif