#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/KILLHandler.h"

#define NICKNAME_PARAM 0

KILLHandler::KILLHandler(IClientDatabase *client_database) :
    client_database_(client_database),
    logger("KILLHandler")
{}

KILLHandler::~KILLHandler()
{}

auto KILLHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();
	
	if (params.size() < 2)
	{
		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
		return;
	}
    if (client->GetType() == IClient::Type::kUnRegistered)
    {
		client->Push(GetErrorMessage(ERR_NOTREGISTERED, "KILL"));
		return ;
    }

	auto nickname = params[NICKNAME_PARAM];
	auto user = dynamic_cast<IUser*>(client);
	if (!user->HasMode(UserMode::UM_OPERATOR))
	{
		client->Push(GetErrorMessage(ERR_NOPRIVILEGES, "SQUIT"));
		return ;
	}

	if (auto otherClient = client_database_->GetClient(nickname))
	{
		if ((*otherClient)->GetType() == IClient::Type::kRemoteServer ||
			(*otherClient)->GetType() == IClient::Type::kLocalServer)
		{
			client->Push(GetErrorMessage(ERR_CANTKILLSERVER));
			return;
		}
		// TODO: Broadcast to other servers?
		client_database_->DisconnectClient((*otherClient)->GetUUID(),
			std::make_optional<std::string>("Has been killed"));
	}
	else
	{
		client->Push(GetErrorMessage(ERR_NOSUCHNICK));
		return;
	}
}
