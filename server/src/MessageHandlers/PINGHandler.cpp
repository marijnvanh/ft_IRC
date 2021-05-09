#include "MessageHandlers/PINGHandler.h"

#define PARAM_SERVER_NAME 0

PINGHandler::PINGHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "PING", 1, true)
{}

PINGHandler::~PINGHandler()
{}

auto PINGHandler::SafeHandle(IMessage &message) -> void
{
	auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();

	client->Push(":" + server_config_->GetName() +
		" " + server_config_->GetName() + " :" + params[PARAM_SERVER_NAME]);
}
