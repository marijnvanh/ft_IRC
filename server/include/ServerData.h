#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include "ClientDatabase.h"
#include "LocalServer.h"
#include <memory>

struct ServerData
{
    ServerData()
    {};

    ClientDatabase client_database_;
    LocalServer local_server_;
};

#endif