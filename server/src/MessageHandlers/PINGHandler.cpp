#include "MessageHandlers/PINGHandler.h"

PINGHandler::PINGHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "PING")
{}

PINGHandler::~PINGHandler()
{}

auto PINGHandler::SafeHandle(IMessage &message) -> void
{
    (void)message;
}
