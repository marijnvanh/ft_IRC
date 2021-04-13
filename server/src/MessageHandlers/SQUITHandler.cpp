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
{}

SQUITHandler::~SQUITHandler()
{}

auto SQUITHandler::HandleUserMessage(IUser *user,
	std::vector<std::string> params) -> void
{
	auto server = client_database_->GetServer(params[SERVER_NAME]);
	if (!server)
	{
		user->Push(GetErrorMessage(ERR_NOSUCHSERVER, params[SERVER_NAME]));
		return ;		
	}
	if ((*server)->GetServerName() == server_config_->GetName())
	{
		user->Push(":" + user->GetNickname() + " NOTICE " + ":Cannot kill host");
		return ;
	}

	if ((*server)->GetType() == IClient::Type::kRemoteServer)
	{
		(*server)->Push(":" + user->GetNickname() + " SQUIT " +
			params[SERVER_NAME] + " :" + params[SQUIT_MESSAGE]);
	}
	else
	{
		(*server)->Disconnect(client_database_,
			std::make_optional<std::string>(params[SQUIT_MESSAGE]));
	}
}

auto SQUITHandler::HandleServerMessage(IServer *server,
	std::vector<std::string> params) -> void
{
	auto other = client_database_->GetServer(params[SERVER_NAME]);
	if (other)
	{
		if ((*other)->GetType() == IClient::Type::kLocalServer)
		{
			(*other)->Disconnect(client_database_, 
				std::make_optional<std::string>(params[SQUIT_MESSAGE]));
		}
		else
		{
			(*other)->Push(":" + server->GetServerName() + " SQUIT " +
				params[SERVER_NAME] + " :" + params[SQUIT_MESSAGE]);
		}
	}
	else if (params[SERVER_NAME] == server_config_->GetName())
	{
		server->Disconnect(client_database_,
			std::make_optional<std::string>(params[SQUIT_MESSAGE]));
	}
	else
		server->Push(GetErrorMessage(ERR_NOSUCHSERVER, params[SERVER_NAME]));
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
	if (!GetCorrectSender(&client, message))
		return ;

	if (client->GetType() == IClient::Type::kLocalServer ||
		client->GetType() == IClient::Type::kRemoteServer)
	{
		HandleServerMessage(dynamic_cast<IServer*>(client), params);
		return;
	}
	HandleUserMessage(dynamic_cast<IUser*>(client), params);
}

auto SQUITHandler::GetCorrectSender(IClient **client, IMessage &message) -> bool
{
	if ((*client)->GetType() == IClient::Type::kLocalServer)
	{
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            (*client)->Push(GetErrorMessage(ERR_NONICKNAMEGIVEN));
            return (false);
        }
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            (*client)->Push(GetErrorMessage(ERR_NOSUCHNICK , *remote_client_nickname));
            return (false);
        }
        *client = *remote_client;
	}
	return (true);
}