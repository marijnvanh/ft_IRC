#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/NOTICEHandler.h"

#define PARAM_NICKNAME 0
#define PARAM_MESSAGE 1

// Configured to not auto-return any (error) messages.
NOTICEHandler::NOTICEHandler(IServerConfig* server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "NOTICE", 0, true)
{}

NOTICEHandler::~NOTICEHandler()
{}

auto NOTICEHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    auto params = message.GetParams();

	// No error handling/returning to disable loops with bot clients.
	if (client->GetType() == IClient::Type::kUnRegistered ||
		params.size() < 2)
	{
		return ;
	}

	auto receiver = client_database_->GetClient(params[PARAM_NICKNAME]);
	if (!receiver)
		return ;

	(*receiver)->Push(":" + client->GetNickname() + " NOTICE " + 
		params[PARAM_MESSAGE]);
}
