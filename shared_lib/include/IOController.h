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
		/**
		 * @brief Construct a new TCP::IOController object
		 */
        IOController();
        ~IOController();

		/**
		 * @brief Checks all FDs in master_fd_list_ and sets corresponding sockets to kReadyToRead state.
		 * 
		 * @exception TCP::IOController::Error when select fails.
		 */
        auto RunOnce() -> void;

		/**
		 * @brief Adds the socket to the list of known sockets, allowing for processing of IO states.
		 * 
		 * @param socket A shared_ptr object of the socket to add.
		 */
		auto AddSocket(std::shared_ptr<Socket> socket) -> void;
		/**
		 * @brief Removes the socket to the list of known sockets, this does not call the Close method on the Socket object.
		 * 
		 * @param socket A shared_ptr object of the socket to remove.
		 */
		auto RemoveSocket(std::shared_ptr<Socket> socket) -> void;

		/**
		 * @brief Attempts to accept new connections from all known listener sockets.
		 * 
		 * @param newSocketCallback A callback which is populated with the newly added socket for non-generic processing.
		 */
        auto AcceptNewConnections(const std::function<void(std::shared_ptr<Socket>)>& newSocketCallback) -> void;

		auto GetSocketsCount() -> int { return this->sockets_.size(); }

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

		/**
		 * @brief Remove any disconnected sockets from map
		 * 
		 */
		auto ValidateSockets() -> void;
    };
} // namespace IRC::TCP

#endif