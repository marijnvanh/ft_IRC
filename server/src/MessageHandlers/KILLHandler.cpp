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

	// TODO: Check if sending client has privileges in the given channel and return ERR_NOPRIVILEGES

	auto nickname = params[NICKNAME_PARAM];

	// Attempt to KILL all clients with the given username.
	if (auto otherClient = client_database_->GetClient(nickname))
	{
		if ((*otherClient)->GetType() == IClient::Type::kRemoteServer ||
			(*otherClient)->GetType() == IClient::Type::kLocalServer)
		{
			client->Push(GetErrorMessage(ERR_CANTKILLSERVER));
			return;
		}

		// TODO: Disconnect the selected user.

	}
	else
	{
		// TODO: Forward to other known servers?
		client->Push(GetErrorMessage(ERR_NOSUCHNICK));
		return;
	}
}
