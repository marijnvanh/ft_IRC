#include <fstream>
#include <iostream>

#include "ServerConfig.h"

using json = nlohmann::json;

ServerConfig::ServerConfig(const std::string &file_path) : config_file_(file_path)
{
	if (this->TryParseFrom(file_path) == false)
	{
		this->server_port_ = DEFAULT_SERVER_PORT;
		this->server_address_ = DEFAULT_SERVER_ADDRESS;
	}
}

auto ServerConfig::ParseServerData(json jf) -> void
{
	if (jf.contains("server-data"))
	{
		this->server_id_ = jf["server-data"]["id"];
		this->server_name_ = jf["server-data"]["name"];
		this->server_network_ = jf["server-data"]["network"];
		this->server_description_ = jf["server-data"]["description"];
	}
}

auto ServerConfig::ParseHostingData(json jf) -> void
{
	if (jf.contains("hosting-data"))
	{
		this->server_port_ = jf["hosting-data"]["port"];
		this->server_address_ = jf["hosting-data"]["address"];
	}
}

auto ServerConfig::TryParseFrom(std::string file_path) -> bool
{
	if (file_path.empty())
	{
		return (false);
	}
	
	std::ifstream ifs(file_path);
	if (!ifs.is_open())
	{
		return (false);
	}

	json jf = json::parse(ifs);

	ParseServerData(jf);
	ParseHostingData(jf);

	// TODO: Parse logging level data?

	return (true);
}
