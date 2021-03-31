#include "LocalServer.h"

LocalServer::LocalServer(std::string server_name, Client &&old_client) :
    IClient(std::move(old_client)),
    Client(std::move(old_client)),
    Server(server_name)
{
    type_ = IClient::Type::kServer;
}

LocalServer::~LocalServer()
{}
