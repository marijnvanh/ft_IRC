#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include "ClientDatabase.h"
#include "LocalServer.h"
#include <memory>

#include "ClientDatabase.h"
#include "ChannelDatabase.h"

struct ServerData
{
    ServerData()
    {};

	ChannelDatabase channel_database_;
    ClientDatabase client_database_;
    LocalServer local_server_;
};

#endif
