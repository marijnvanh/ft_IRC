#include "MessageHandlers/PingPongHandler.h"

auto PingPongHandler(std::shared_ptr<ClientDatabase> client_database, Message message) -> void
{
    auto client = client_database->GetClient(message.GetUUID());
    client->Access([](IClient &client)
    {
        client.Push(std::make_shared<std::string>("pong"));
    });
}
