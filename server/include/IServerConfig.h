#ifndef ISERVER_CONFIG_HPP
#define ISERVER_CONFIG_HPP

#include <string>

#include "nlohmann/json.hpp"

#define DEFAULT_SERVER_PORT "5000"
#define DEFAULT_SERVER_ADDRESS "0.0.0.0"

struct IServerConfig
{

public:

	auto SetId(std::string server_id) -> void { server_id_ = server_id; }
	auto SetName(std::string server_name) -> void { server_name_ = server_name; }
	auto SetNetwork(std::string server_network) -> void { server_network_ = server_network; }
	auto SetDescription(std::string server_description) -> void { server_description_ = server_description; }

	auto SetPort(std::string server_port) -> void { server_port_ = server_port; }
	auto SetAddress(std::string server_address) -> void { server_address_ = server_address; }

	auto GetId() -> const std::string { return (server_id_); }
	auto GetName() -> const std::string { return (server_name_); }
	auto GetNetwork() -> const std::string { return (server_network_); }
	auto GetDescription() -> const std::string { return (server_description_); }

	auto GetPort() -> std::string { return (server_port_); }
	auto GetAddress() -> std::string { return (server_address_); }

	auto GetAdministrators() -> std::unordered_map<std::string, std::string>& { return (administrators_); }
	auto GetAuthorizedServers() -> std::unordered_map<std::string, std::pair<std::string, std::string>>& { return (authorized_servers_); }

protected:

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

    /**
     * @brief List of administrators
     */
	std::unordered_map<std::string, std::string> administrators_;

    /**
     * @brief List of authorized servers
     */
	std::unordered_map<std::string, std::pair<std::string, std::string>> authorized_servers_;
};

#endif
