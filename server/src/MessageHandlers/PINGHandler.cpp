#include "MessageHandlers/PINGHandler.h"

PINGHandler::PINGHandler(IClientDatabase *client_database)
{
    (void)client_database;
}

PINGHandler::~PINGHandler()
{}

auto PINGHandler::Handle(IMessage &message) -> void
{
    (void)message;
}
