#include <fstream>
#include <iostream>

#include "ServerConfig.h"

using json = nlohmann::json;

#define LOG_NAME 0
#define LOG_LEVEL 1

#define SERVER_NAME 0
#define IP 1
#define PORT 2

#define USERNAME 0
#define PASSWORD 1

ServerConfig::ServerConfig(const std::string &file_path) :
	config_file_(file_path),
	logger("ServerConfig")
{
	if (this->TryParseFrom(file_path) == false)
	{
		this->ping_time_ = DEFAULT_PING_TIME;
		this->server_port_ = DEFAULT_SERVER_PORT;
		this->server_ssl_port_ = DEFAULT_SSL_SERVER_PORT;
		this->server_address_ = DEFAULT_SERVER_ADDRESS;
		this->server_ssl_crt_ = DEFAULT_SSL_CRT;
		this->server_ssl_key_ = DEFAULT_SSL_KEY;
	}
}

auto ServerConfig::ParsePingData(json jf) -> void
{
	this->ping_time_ = DEFAULT_PING_TIME;
	if (jf.contains("ping-time"))
	{
		this->ping_time_ = jf["ping-time"];
	}
}

auto ServerConfig::ParseServerData(json jf) -> void
{
	if (jf.contains("server-data"))
	{
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
		this->server_ssl_port_ = jf["hosting-data"]["ssl_port"];
		this->server_address_ = jf["hosting-data"]["address"];
		this->server_ssl_crt_ = jf["hosting-data"]["ssl_crt"];
		this->server_ssl_key_ = jf["hosting-data"]["ssl_key"];
	}
}

auto ServerConfig::ParseLogData(json jf) -> void
{
	if (jf.contains("log-settings"))
	{
		auto default_log_level_str = jf["log-settings"]["default-level"];
		logger.SetDefaultLogLevel(logger.ResolveLogLevel(default_log_level_str));

		auto log_settings = jf["log-settings"]["log-levels"];
		for (auto it = log_settings.cbegin(); it < log_settings.cend(); it++)
		{
			auto log_level = logger.ResolveLogLevel((*it)[LOG_LEVEL]);
			logger.SetLevel((*it)[LOG_NAME], log_level);
		}
	}
}

auto ServerConfig::ParseAdministrators(json jf) -> void
{
	if (jf.contains("administrators"))
	{
		auto administrators = jf["administrators"];
		for (auto it = administrators.cbegin(); it < administrators.cend(); it++)
		{
			administrators_.insert(std::make_pair((*it)[USERNAME], (*it)[PASSWORD]));
		}
	}
}

auto ServerConfig::ParseAuthorizedServers(json jf) -> void
{
	if (jf.contains("authorized-servers"))
	{
		auto authorized_servers = jf["authorized-servers"].get<json::object_t>();
        for (auto& kvp : authorized_servers)
        {
			auto conf = std::make_unique<ServerConnectData>();

			conf->SetName(kvp.first);
			conf->SetIP(kvp.second.at("ip"));
			conf->SetPort(kvp.second.at("port"));
			if (kvp.second.contains("recv_pass"))
				conf->SetRecvPass(kvp.second.at("recv_pass"));
			if (kvp.second.contains("send_pass"))
				conf->SetSendPass(kvp.second.at("send_pass"));
			
			authorized_servers_.insert(std::make_pair(
				kvp.first,
				std::move(conf)));
		}
	}
}

auto ServerConfig::TryParseFrom(std::string file_path) -> bool
{
	logger.Log(LogLevel::INFO, "Loading server config from %s", file_path.c_str());
	if (file_path.empty())
	{
		return (false);
	}
	
	std::ifstream ifs(file_path);
	if (!ifs.is_open())
	{
		return (false);
	}
	try {
		json jf = json::parse(ifs);
		ParsePingData(jf);
		ParseServerData(jf);
		ParseHostingData(jf);
		ParseAdministrators(jf);
		ParseLogData(jf);
		ParseAuthorizedServers(jf);
	}
	catch (std::exception &ex) {
		logger.Log(LogLevel::ERROR, "Failed to parse config file");
		return (false);
	}

	return (true);
}
