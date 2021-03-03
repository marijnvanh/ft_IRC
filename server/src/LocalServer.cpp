#include "LocalServer.h"

LocalServer::LocalServer() : uuid_(IRC::UUIDGenerator::GetInstance().Generate())
{}

LocalServer::~LocalServer()
{}

auto LocalServer::AddClient(std::shared_ptr<IClient> client) -> void
{
    if (clients_.find(client->GetUUID()) != clients_.end())
        throw std::runtime_error("Why are we trying to add user twice to server?"); //Should never happen

    clients_.insert(std::make_pair(client->GetUUID(), client));
}

auto LocalServer::RemoveClient(IRC::UUID uuid) -> void
{
	clients_.erase(uuid);
}
