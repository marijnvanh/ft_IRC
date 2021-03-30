#ifndef SERVER_DATA_HPP
#define SERVER_DATA_HPP

#include <memory>

#include "ServerConfig.h"
#include "ClientDatabase.h"
#include "ChannelDatabase.h"

struct ServerData
{
    ServerData(const std::string &config_file) : server_config_(config_file)
    {};

	ServerConfig server_config_;

    ClientDatabase client_database_;
	ChannelDatabase channel_database_;
};

#endif
