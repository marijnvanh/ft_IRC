#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include "Resolver.h"
#include "Socket.h"

#include <map>
#include <queue>
#include <memory>

namespace TCP
{
    class IOController
    {
    private:
        std::map<int, std::unique_ptr<Socket>> sockets_;
        std::queue<std::string> &message_queue_;

        fd_set master_fd_list_;
        int max_fd_;
        Socket listener_;

        void AcceptNewConnection();
        void ReadSocket(int socket_fd);

    public:
        IOController(AddressInfo &address_info,
                     std::queue<std::string> &message_queue,
                     int backlog = DEFAULT_BACKLOG);
        ~IOController();
        void RunOnce(int timeout);
    };
} // namespace TCP

#endif