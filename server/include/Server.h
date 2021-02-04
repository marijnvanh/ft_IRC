#ifndef __SERVER_H__
#define __SERVER_H__

#include <memory>
#include <vector>

#include "Socket.h"
#include "AddressInfo.h"
#include "IOController.h"
#include "ClientDatabase.h"

#define PORT "5000" //TODO remove this

using namespace IRC;

class Server
{
    public:

    Server();
    ~Server();

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
    ClientDatabase client_database_;
    TCP::IOController tcp_io_controller_;
    
};

#endif