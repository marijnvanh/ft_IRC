#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <string>

#include "IServerConfig.h"
#include "nlohmann/json.hpp"

#define DEFAULT_SERVER_PORT "5000"
#define DEFAULT_SERVER_ADDRESS "0.0.0.0"

struct ServerConfig : public IServerConfig
{

public:
	
	ServerConfig();
	ServerConfig(std::string file_path);

	auto TryParseFrom(std::string file_path) -> bool;

private:

	auto ParseServerData(nlohmann::json jf) -> void;
	auto ParseHostingData(nlohmann::json jf) -> void;
};

#endif
