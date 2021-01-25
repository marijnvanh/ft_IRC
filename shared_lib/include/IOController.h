#ifndef __IOCONTROLLER_H__
#define __IOCONTROLLER_H__

#include "AddressInfo.h"
#include "Socket.h"

#include <map>
#include <memory>
#include <functional>

namespace IRC::TCP
{
    class IOController
    {
    public:
        IOController();
        ~IOController();

        auto RunOnce() -> void;

		auto AddSocket(std::shared_ptr<Socket> socket) -> void;
		auto RemoveSocket(std::shared_ptr<Socket> socket) -> void;

        auto AcceptNewConnections(const std::function<void(std::shared_ptr<Socket>)>& newSocketCallback) -> void;

        class Error : public std::runtime_error
        {
        public:
            Error(const char *msg) : std::runtime_error(msg) {}
        };

    private:
        std::map<int, std::shared_ptr<Socket>> sockets_;

        int max_fd_;
        fd_set master_fd_list_;

		/**
		 * @brief Updates known socket states. Removes socket from known sockets if its state is kDisconnected.
		 * 
		 * @param ready_fds A set of fds that contain current states of socket fds.
		 */
		auto UpdateSocketStates(fd_set *ready_fds) -> void;
    };
} // namespace IRC::TCP

#endif