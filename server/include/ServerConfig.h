#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <string>

#include "IServerConfig.h"
#include "nlohmann/json.hpp"
#include "Logger.h"

#define DEFAULT_SERVER_PORT "5000"
#define DEFAULT_SERVER_ADDRESS "0.0.0.0"

struct ServerConfig : public IServerConfig
{

public:
	
	ServerConfig(const std::string &file_path);

	auto TryParseFrom(std::string file_path) -> bool;

private:
	std::string config_file_;
	Logger logger;

	auto ParseServerData(nlohmann::json jf) -> void;
	auto ParseHostingData(nlohmann::json jf) -> void;
	auto ParseLogData(nlohmann::json jf) -> void;
	auto ParseAuthorizedServers(nlohmann::json jf) -> void;
};

#endif
