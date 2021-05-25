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

	auto GetAuthorizedServer(const std::string &name) -> std::optional<ServerConnectData*> override
	{
		auto server = authorized_servers_.find(name);
		if (server != authorized_servers_.end())
		{
			return (std::make_optional<ServerConnectData*>(server->second.get()));
		}
		return (std::nullopt);
	};

private:
	std::string config_file_;
	Logger logger;

	auto ParsePingData(nlohmann::json jf) -> void;
	auto ParseServerData(nlohmann::json jf) -> void;
	auto ParseHostingData(nlohmann::json jf) -> void;
	auto ParseLogData(nlohmann::json jf) -> void;
	auto ParseAdministrators(nlohmann::json jf) -> void;
	auto ParseAuthorizedServers(nlohmann::json jf) -> void;
};

#endif
