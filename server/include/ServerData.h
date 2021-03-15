#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include <memory>

#include "ClientDatabase.h"
#include "ChannelDatabase.h"

struct ServerData
{
    ServerData() : 
		client_database_(std::make_shared<ClientDatabase>()),
		channel_database_(std::make_shared<ChannelDatabase>()) {};

    std::shared_ptr<ClientDatabase> client_database_;
    std::shared_ptr<ChannelDatabase> channel_database_;
};

#endif
