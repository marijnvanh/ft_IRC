#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/KILLHandler.h"

#define NICKNAME_PARAM 0
#define MESSAGE_PARAM 1

#define DEFAULT_KILL_MESSAGE "Has been killed"

KILLHandler::KILLHandler(IClientDatabase *client_database) :
    client_database_(client_database),
    logger("KILLHandler")
{}

KILLHandler::~KILLHandler()
{}
#include <iostream>
auto KILLHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();

	if (params.size() < 2)
	{
		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS, "KILL"));
		return ;
	}
    if (client->GetType() == IClient::Type::kUnRegistered)
    {
		client->Push(GetErrorMessage(ERR_NOTREGISTERED, "KILL"));
		return ;
    }
	if (!GetCorrectSender(client, message))
		return ;

	auto nickname = params[NICKNAME_PARAM];
	if (auto otherUser = client_database_->GetUser(nickname))
	{
		if ((*otherUser)->GetType() == IClient::Type::kRemoteUser)
			HandleKillForRemoteUser(client, *otherUser, params);
		else
			HandleKillForLocalUser(client, *otherUser, params);
	}
	else
		client->Push(GetErrorMessage(ERR_NOSUCHNICK, "KILL"));
}

auto KILLHandler::HandleKillForLocalUser(IClient *client, IUser *otherUser,
	std::vector<std::string> params) -> void
{
	auto user = dynamic_cast<IUser*>(client);
	if (!user->HasMode(UserMode::UM_OPERATOR))
	{
		client->Push(GetErrorMessage(ERR_NOPRIVILEGES, "KILL"));
		return ;
	}

	auto message = params.size() <= MESSAGE_PARAM ?
		DEFAULT_KILL_MESSAGE : params[MESSAGE_PARAM];

	// Disconnecting the client should broadcast all required messages.
	client_database_->DisconnectClient(otherUser->GetUUID(),
		std::make_optional<std::string>(message));	
}

auto KILLHandler::HandleKillForRemoteUser(IClient *client, IUser *otherUser,
	std::vector<std::string> params) -> void
{
	auto user = dynamic_cast<IUser*>(client);
	if (!user->HasMode(UserMode::UM_OPERATOR))
	{
		client->Push(GetErrorMessage(ERR_NOPRIVILEGES, "KILL"));
		return ;
	}

	auto message = params.size() <= MESSAGE_PARAM ?
		DEFAULT_KILL_MESSAGE : params[MESSAGE_PARAM];

	otherUser->Push(":" + client->GetNickname() + " KILL :" + message);
}

auto KILLHandler::GetCorrectSender(IClient *client, IMessage &message) -> bool
{
	if (client->GetType() == IClient::Type::kLocalServer)
	{
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(GetErrorMessage(ERR_NONICKNAMEGIVEN));
            return (false);
        }
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(GetErrorMessage(ERR_NOSUCHNICK , *remote_client_nickname));
            return (false);
        }
        client = *remote_client;
	}
	return (true);
}
