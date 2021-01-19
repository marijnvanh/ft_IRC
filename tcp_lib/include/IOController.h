#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include "AddressInfo.h"
#include "Socket.h"
#include "Message.h"

#include <map>
#include <memory>

#define DEFAULT_RETRIES 3

namespace TCP
{
    class IOController
    {
    public:
        IOController(int max_retries = DEFAULT_RETRIES);
        ~IOController();

        auto RunOnce() -> void;

		auto AddSocket(std::shared_ptr<Socket> socket) -> void;

        auto AcceptNewConnections(void (*f)(std::shared_ptr<Socket>)) -> void;

        class Error : public std::runtime_error
        {
        public:
            Error(const char *msg) : std::runtime_error(msg) {}
        };

    private:
        enum Type { kServer, kClient };

        std::map<int, std::shared_ptr<Socket>> sockets_;

        int max_fd_;
        int max_retries_;
        fd_set master_fd_list_;

        auto SendMessage(TCP::Message &message, fd_set *write_fds) -> void;
        auto ReadFromSocket(int socket_fd) -> void;
        auto ValidateSocket(std::shared_ptr<const Socket> socket) -> std::shared_ptr<Socket>;

        auto DeleteSocket(int socket_fd) -> void;

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
    };
} // namespace TCP

#endif