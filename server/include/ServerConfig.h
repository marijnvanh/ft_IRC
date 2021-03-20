#ifndef SERVER_CONFIG_HPP
#define SERVER_CONFIG_HPP

#include <string>

#include "nlohmann/json.hpp"

#define DEFAULT_SERVER_PORT "5000"
#define DEFAULT_SERVER_ADDRESS "0.0.0.0"

struct ServerConfig
{

public:
	
	ServerConfig();
	ServerConfig(std::string file_path);

	auto TryParseFrom(std::string file_path) -> bool;

	auto GetId() -> const std::string { return (server_id_); }
	auto GetName() -> const std::string { return (server_name_); }
	auto GetNetwork() -> const std::string { return (server_network_); }
	auto GetDescription() -> const std::string { return (server_description_); }

	auto GetPort() -> std::string { return (server_port_); }
	auto GetAddress() -> std::string { return (server_address_); }

private:

	auto ParseServerData(nlohmann::json jf) -> void;
	auto ParseHostingData(nlohmann::json jf) -> void;

    /**
     * @brief ID of the server, 3 characters long.
	 * 1st char should be a number, 2nd and 3rd should be either number or letter.
     */
	std::string server_id_;

    /**
     * @brief Name of the server, tends to be similar (if not equal) to server_network.
     */
	std::string server_name_;

    /**
     * @brief Network of the server, tends to be a URL based string (IE tuts.ub.com)
     */
	std::string server_network_;

    /**
     * @brief Description of the server, tends to be a sort of welcome message.
     */
	std::string server_description_;


    /**
     * @brief Port the server socket should bind to (defaults to DEFAULT_SERVER_PORT).
     */
	std::string server_port_;
	
    /**
     * @brief Address the server socket should bind to (defaults to DEFAULT_SERVER_ADDRESS).
     */
	std::string server_address_;
};

#endif
