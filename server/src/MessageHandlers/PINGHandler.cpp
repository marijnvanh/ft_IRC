#include "MessageHandlers/PINGHandler.h"

PINGHandler::PINGHandler(IClientDatabase *client_database) :
	CommandHandler(client_database, "PING")
{}

PINGHandler::~PINGHandler()
{}

auto PINGHandler::SafeHandle(IMessage &message) -> void
{
    (void)message;
}
