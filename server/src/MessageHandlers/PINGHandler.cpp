#include "MessageHandlers/PINGHandler.h"

PINGHandler::PINGHandler(IServerConfig *server_config, IClientDatabase *client_database)
    : server_config_(server_config)
{
    (void)client_database;
    (void)server_config_;
}

PINGHandler::~PINGHandler()
{}

auto PINGHandler::Handle(IMessage &message) -> void
{
    (void)message;
}
