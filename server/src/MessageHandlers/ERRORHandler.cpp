#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/ERRORHandler.h"

#define PARAM_ERROR_MESSAGE 0

ERRORHandler::ERRORHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "ERROR", 1)
{}

ERRORHandler::~ERRORHandler()
{}

auto ERRORHandler::SafeHandle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	/* If ERROR does not come from a server we ignore it */
	if (client->GetType() != IClient::Type::kLocalServer) {

    	logger_.Log(LogLevel::ERROR, "Received ERROR from non server: %s", params[PARAM_ERROR_MESSAGE].c_str());
		return;
	}

    logger_.Log(LogLevel::ERROR, "Received ERROR: %s", params[PARAM_ERROR_MESSAGE].c_str());
}
