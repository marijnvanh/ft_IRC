#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include "AddressInfo.h"
#include "Socket.h"
#include "Message.h"

#include <map>
#include <memory>
#include <functional>

#define DEFAULT_RETRIES 3

namespace IRC::TCP
{
    class IOController
    {
    public:
        IOController(int max_retries = DEFAULT_RETRIES);
        ~IOController();

        auto RunOnce() -> void;

		auto AddSocket(std::shared_ptr<Socket> socket) -> void;

        auto AcceptNewConnections(const std::function<void(std::shared_ptr<Socket>)>& newSocketCallback) -> void;

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

        auto ReadFromSocket(int socket_fd) -> void;
        auto ValidateSocket(std::shared_ptr<const Socket> socket) -> std::shared_ptr<Socket>;

		/**
		 * @brief Updates known socket states. Removes socket from known sockets if its state is kDisconnected.
		 * 
		 * @param ready_fds A set of fds that contain current states of socket fds.
		 */
		auto UpdateSocketStates(fd_set *ready_fds) -> void;

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
} // namespace IRC::TCP

#endif