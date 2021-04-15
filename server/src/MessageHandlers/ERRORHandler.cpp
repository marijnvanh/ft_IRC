#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/ERRORHandler.h"

#define PARAM_ERROR_MESSAGE 0

ERRORHandler::ERRORHandler(IClientDatabase *client_database)
	: client_database_(client_database),
    logger("ERRORHandler")
{}

ERRORHandler::~ERRORHandler()
{}

auto ERRORHandler::Handle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	/* If ERROR does not come from a server we ignore it */
	if (client->GetType() != IClient::Type::kLocalServer)
		return;

	if (params.size() == 0)
	{
		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS, "ERROR"));
		return;
	}
    logger.Log(LogLevel::ERROR, "Received ERROR: %s", params[PARAM_ERROR_MESSAGE].c_str());
}
