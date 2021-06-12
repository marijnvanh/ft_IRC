#include "Server.h"

#define DEFAULT_DISCONNECT "Server killed"

Server::Server(std::string server_name) : server_name_(server_name)
{}

Server::~Server()
{}

auto Server::AddClient(IClient* client) -> void
{
    if (clients_.find(client->GetUUID()) != clients_.end())
        throw std::runtime_error("Why are we trying to add user twice to server?"); //Should never happen

    clients_.insert(std::make_pair(client->GetUUID(), client));
}

auto Server::RemoveClient(IRC::UUID uuid) -> void
{
	clients_.erase(uuid);
}

auto Server::Disconnect(IClientDatabase *client_database,
	std::optional<std::string> message) -> void
{
	if (!message)
	{
		message = std::make_optional<std::string>(DEFAULT_DISCONNECT);
	}

    for (auto it = clients_.begin(), next_it = it; it != clients_.end(); it = next_it)
    {
        ++next_it;
        client_database->DisconnectClient(it->first, message);
    }
    /*
        Remove server from local and remote server lists
        Only remote servers will have local_server_ and remote_server_ set
        local_server_ and remote_server_ can be the same so we only delete a server once in that case
    */
    if (local_server_)
    {
        if (remote_server_ && remote_server_ != local_server_)
            remote_server_->RemoveClient(uuid_);
        local_server_->RemoveClient(uuid_);
    }
}

/**
 * @brief 
 * 
 * @param this_server_name // Hacked this in here because IServer doesn't know about our server
 * @return std::string 
 */
auto Server::GenerateServerMessage(const std::string &this_server_name) const -> std::string
{
	std::string server_message;

    if (type_ == IClient::Type::kLocalServer)
        server_message = ":" + this_server_name;
    else
        server_message = ":" + remote_server_->GetServerName();

    server_message += " SERVER " + server_name_ + " " +
		std::to_string(GetHops() + 1) + " " + std::to_string(GetOurToken()) +
		" :Unknown description"; 

    return server_message;
}
