#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include "ClientDatabase.h"
#include "LocalServer.h"
#include <memory>
//TODO make shared unique
struct ServerData
{
    ServerData() :
        client_database_(std::make_shared<ClientDatabase>()),
        local_server_(std::make_shared<LocalServer>())
    {};

    std::shared_ptr<ClientDatabase> client_database_;
    std::shared_ptr<LocalServer> local_server_;
    
};

#endif