#ifndef __IRCSERVER_H__
#define __IRCSERVER_H__

#include <memory>
#include <vector>

#include "Socket.h"
#include "AddressInfo.h"
#include "IOController.h"
#include "ServerData.h"
#include "IMessageDispatcher.h"
#include "Logger.h"

#define PORT "5000" //TODO remove this

using namespace IRC;

class IRCServer
{
    public:

    IRCServer();
    ~IRCServer();

    /**
     * @brief Init listener socket and io controller
     * 
     * @param address 
     */
    auto Start(std::string address) -> void;

    /**
     * @brief Run server loop once
     * 
     */
    auto RunOnce() -> void;

    private:

    std::unique_ptr<ServerData> server_data_;
    std::unique_ptr<IMessageDispatcher> message_dispatcher_;
    TCP::IOController tcp_io_controller_;
    Logger logger;
};

#endif