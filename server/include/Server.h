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

    auto Start(std::string address) -> void;

    auto RunOnce() -> void;

    private:
    ClientDatabase client_database_;
    TCP::IOController io_controller_;
    
};

#endif