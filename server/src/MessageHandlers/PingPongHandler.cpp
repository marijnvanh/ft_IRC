#include "MessageHandlers/PingPongHandler.h"

auto PingPongHandler(IClientDatabase *client_database, Message message) -> void
{
    (void)client_database;
    auto client = *(client_database->GetClient(message.GetClientUUID()));
    
    client->Push("pong");
}
