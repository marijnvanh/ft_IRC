#include "Server.h"

Server::Server(Client &&old_client) : Client(std::move(old_client))
{
    type_ = IClient::Type::kServer;
}

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
