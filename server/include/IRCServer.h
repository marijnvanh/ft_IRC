#ifndef __IRCSERVER_H__
#define __IRCSERVER_H__

#include <memory>
#include <vector>
#include <time.h>
#include <optional>

#include "Logger.h"
#include "ServerData.h"
#include "IOController.h"
#include "IMessageDispatcher.h"

using namespace IRC;

class IRCServer
{
    public:

    IRCServer(const std::string &config_path);
    ~IRCServer();

    /**
     * @brief Init listener socket and io controller
     */
    auto Start() -> void;

    /**
     * @brief Run server loop once
     * 
     */
    auto RunOnce() -> void;

    /**
     * @brief Create a new connection
     * 
     * @param ip 
     * @param port 
     * @return IClient* 
     */
    auto CreateNewConnection(std::string &ip, std::string &port) -> std::optional<IClient*>;

	auto GetCurrentSecond() -> time_t
	{
		return time_.tv_sec;
	}

    private:

	struct timespec time_;

    std::unique_ptr<ServerData> server_data_;
    std::unique_ptr<IMessageDispatcher> message_dispatcher_;
    TCP::IOController tcp_io_controller_;
    Logger logger;

	auto PingClients() -> void;
};

#endif