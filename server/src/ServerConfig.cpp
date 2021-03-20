#include <fstream>

#include "ServerConfig.h"
#include "nlohmann/json.hpp"

ServerConfig::ServerConfig() : server_port_(DEFAULT_SERVER_PORT), server_address_(DEFAULT_SERVER_ADDRESS)
{
}

ServerConfig::ServerConfig(std::string file_path)
{
	this->ParseFrom(file_path);
}

auto ServerConfig::ParseFrom(std::string file_path) -> void
{
	using json = nlohmann::json;
	// TODO: Parse server data.
	// TODO: Parse hosting data.

	// TODO: Parse logging leve data?
	
	std::ifstream ifs(file_path);
	json jf = json::parse(ifs);
}
