#include "MessageHandlers/PONGHandler.h"

PONGHandler::PONGHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "PONG", 1, true)
{}

PONGHandler::~PONGHandler()
{}

auto PONGHandler::SafeHandle(IMessage &message) -> void
{
	auto client = *(client_database_->GetClient(message.GetClientUUID()));

	if (client->GetType() == IClient::Type::kLocalUser)
	{
		// Could also do a PONG overload/spam protection here?
		client->SetLastPing(true);
	}
}
