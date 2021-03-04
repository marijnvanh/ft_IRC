#include "Numerics.h"
#include "MessageHandlers/KILLHandler.h"

#define NICKNAME_PARAM 0

auto KILLHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

	auto params = message.GetParams();
	if (params.size() < 2)
	{
		client->Push(std::to_string(ERR_NEEDMOREPARAMS));
		return;
	}

	// TODO: Check if sending client has privileges in the given channel and return ERR_NOPRIVILEGES

	auto nickname = params[NICKNAME_PARAM];

	// Attempt to KILL all clients with the given username.
	if (auto otherClient = client_database->GetClient(nickname))
	{
		if ((*otherClient)->GetType() == IClient::Type::kServer)
		{
			client->Push(std::to_string(ERR_CANTKILLSERVER));
			return;
		}

		// TODO: Disconnect the selected user.

	}
	else
	{
		// TODO: Forward to other known servers?
		client->Push(std::to_string(ERR_NOSUCHNICK));
		return;
	}
}
