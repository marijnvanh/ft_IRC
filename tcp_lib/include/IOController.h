#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include "Resolver.h"
#include "Socket.h"
#include "Message.h"

#include <map>
#include <queue>
#include <memory>

namespace TCP
{
    class IOController
    {
    private:
        std::map<int, std::unique_ptr<Socket>> sockets_;
        std::queue<TCP::Message> &recv_message_queue_;
        std::queue<TCP::Message> &send_message_queue_;

        fd_set master_fd_list_;
        int max_fd_;
        Socket listener_;

        void HandleSendQueue(struct timeval *time_val);
        void SendMessage(const Socket &socket, std::string &data);
        void HandleRecvQueue(struct timeval *time_val);
        void AcceptNewConnection();
        void ReadSocket(int socket_fd);

    public:
        IOController(AddressInfo &address_info,
                     std::queue<TCP::Message> &recv_message_queue,
                     std::queue<TCP::Message> &send_message_queue,
                     int backlog = DEFAULT_BACKLOG);
        ~IOController();
        void RunOnce(int timeout);

        class Error : public std::runtime_error
        {
        public:
            Error(const char *msg) : std::runtime_error(msg) {}
        };

    };
} // namespace TCP

#endif