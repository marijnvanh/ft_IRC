#include "IServer.h"
#include "Numerics.h"
#include "Utilities.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"
#include "MessageHandlers/SQUITHandler.h"

#define DEFAULT_SQUIT_MESSAGE "Server exiting"

#define SERVER_NAME		0
#define SQUIT_MESSAGE	1

SQUITHandler::SQUITHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "SQUIT", 2)
{}

SQUITHandler::~SQUITHandler()
{}

auto SQUITHandler::HandleUserMessage(IUser *user,
	std::vector<std::string> params) -> void
{
	auto server = client_database_->GetServer(params[SERVER_NAME]);

	if (!user->HasMode(UserMode::UM_OPERATOR))
	{
		user->Push(GetErrorMessage(server_config_->GetName(), user->GetPrefix(), ERR_NOPRIVILEGES, "SQUIT"));
		return ;
	}
	if (params[SERVER_NAME] == server_config_->GetName())
	{
		user->Push(":" + user->GetNickname() + " NOTICE :Cannot kill host");
		return ;
	}
	if (!server)
	{
		user->Push(GetErrorMessage(server_config_->GetName(), user->GetPrefix(), ERR_NOSUCHSERVER, params[SERVER_NAME]));
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
	auto target_server = client_database_->GetServer(params[SERVER_NAME]);
	if (target_server)
	{
		if ((*target_server)->GetType() == IClient::Type::kLocalServer ||
			(*target_server)->GetLocalServer() == server)
		{
			client_database_->DisconnectServer(*target_server,
				std::make_optional<std::string>(params[SQUIT_MESSAGE]));
		}
		else
		{
			(*target_server)->Push(":" + server->GetServerName() + " SQUIT " +
				params[SERVER_NAME] + " :" + params[SQUIT_MESSAGE]);			
		}
	}
	else if (params[SERVER_NAME] == server_config_->GetName())
	{
		server->Disconnect(client_database_,
			std::make_optional<std::string>(params[SQUIT_MESSAGE]));
	}
	else {

		server->Push(FormatERRORMessage(server->GetPrefix(), "SQUIT No such server: " + params[SERVER_NAME]));
	}
}

auto SQUITHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();

	if (!GetOriginalSender(&client, message))
		return ;

	if (client->GetType() == IClient::Type::kLocalServer ||
		client->GetType() == IClient::Type::kRemoteServer)
	{
		HandleServerMessage(dynamic_cast<IServer*>(client), params);
		return;
	}
	HandleUserMessage(dynamic_cast<IUser*>(client), params);
}

auto SQUITHandler::GetOriginalSender(IClient **client, IMessage &message) -> bool
{
	if ((*client)->GetType() == IClient::Type::kLocalServer)
	{
        auto prefix = message.GetPrefix();

		if (!prefix)
		{
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "SQUIT No nickname given"));
			return (false);
		}

		auto original_client = client_database_->GetClient(*prefix);
		if (!original_client)
		{
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "SQUIT Client could not be resolved: " + *prefix));
		}
		*client = *original_client;
	}
	return (true);
}
