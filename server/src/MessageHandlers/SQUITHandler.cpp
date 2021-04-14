#include "IServer.h"
#include "Numerics.h"
#include "Utilities.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"
#include "MessageHandlers/SQUITHandler.h"

#define DEFAULT_SQUIT_MESSAGE "Server exiting"

#define SERVER_NAME		0
#define SQUIT_MESSAGE	1

SQUITHandler::SQUITHandler(IServerConfig *server_config,
	IClientDatabase *client_database) :
	server_config_(server_config),
    client_database_(client_database),
    logger("SQUITHandler")
{
	(void)server_config_;
}

SQUITHandler::~SQUITHandler()
{}

auto SQUITHandler::HandleUserMessage(IUser *user,
	std::vector<std::string> params) -> void
{
	(void)user;
	(void)params;
}

auto SQUITHandler::HandleServerMessage(IServer *server,
	std::vector<std::string> params) -> void
{
	(void)server;
	(void)params;
}

auto SQUITHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();
	
	if (params.size() < 2)
	{
		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS, "SQUIT"));
		return;
	}
    if (client->GetType() == IClient::Type::kUnRegistered)
    {
		client->Push(GetErrorMessage(ERR_NOTREGISTERED, "SQUIT"));
		return ;
    }
	
	// TODO: Handle user message.
	// TODO: Handle server message.
}
