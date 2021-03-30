#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/SERVERHandler.h"
#include "RemoteServer.h"

#define PARAM_SERVER_NAME 0
#define PARAM_HOPCOUNT 1
#define PARAM_INFO 2

SERVERHandler::SERVERHandler(IClientDatabase *client_database)
	: client_database_(client_database),
    logger("SERVERHandler")
{}

SERVERHandler::~SERVERHandler()
{}

auto SERVERHandler::Handle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	if (client->GetState() == IClient::kRegistered && client->GetType() != IClient::Type::kServer )
	{
		client->Push(GetErrorMessage(ERR_ALREADYREGISTERED));
		return;
	}	
	if (params.size() != 3)
	{
		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
		return;
	}
    
    /* Check if server already exists, if yes, disconnect current connection */
    auto server = client_database_->GetServer(params[PARAM_SERVER_NAME]);
    if (server)
    {
        client_database_->DisconnectClient(message.GetClientUUID());
        return ;
    }

    if (client->GetType() == IClient::Type::kServer)
    {
        auto remote_server_name = message.GetServername();
        if (!remote_server_name)
        {
    		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
            return ;
        }
        auto remote_server = client_database_->GetServer(*remote_server_name);
        if (!remote_server)
        {
    		client->Push(GetErrorMessage(ERR_NOSUCHSERVER, *remote_server_name));
            return ;
        }
        auto local_server = (*client_database_->GetServer(client->GetUUID()));
        HandleRemoteServerRegistration(local_server, *remote_server, params[PARAM_SERVER_NAME]);
    }
    else
        HandleLocalServerRegistration(client, params[PARAM_SERVER_NAME]);
    //TODO broadcast server message to all local connected servers
}

auto SERVERHandler::HandleLocalServerRegistration(IClient *client, const std::string &server_name) -> void
{
    client_database_->RegisterLocalServer(server_name, client->GetUUID());
}

auto SERVERHandler::HandleRemoteServerRegistration(IServer *local_server, IServer *remote_server, const std::string &server_name) -> void
{
    auto new_server = std::make_unique<RemoteServer>(local_server, remote_server, server_name);
    client_database_->AddServer(std::move(new_server));
}
