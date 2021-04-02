#include "RemoteServer.h"

RemoteServer::RemoteServer(IServer* local_server, IServer* remote_server, std::string server_name) :
    Server(server_name),
    uuid_(IRC::UUIDGenerator::GetInstance().Generate())
{
    local_server_ = local_server;
    remote_server_ = remote_server;
}

RemoteServer::~RemoteServer()
{}

auto RemoteServer::Push(std::string irc_message) -> void
{
    local_server_->Push(irc_message);
}
