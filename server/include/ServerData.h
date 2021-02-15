#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include "ClientDatabase.h"
#include <memory>

struct ServerData
{
    ServerData() : client_database_(std::make_shared<ClientDatabase>()) {};

    std::shared_ptr<ClientDatabase> client_database_;
};

#endif