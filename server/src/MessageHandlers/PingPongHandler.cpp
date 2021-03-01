#include "MessageHandlers/PingPongHandler.h"

auto PingPongHandler(std::shared_ptr<IClientDatabase> client_database, Message message) -> void
{
    (void)client_database;
    auto client = message.GetClient();
    
    client->Push("pong");
}
