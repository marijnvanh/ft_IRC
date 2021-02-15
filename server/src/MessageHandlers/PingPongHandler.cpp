#include "MessageHandlers/PingPongHandler.h"

//TODO Change to only take IRCMessage
//TODO Get UUID from message
//TODO Send ping
auto PingPongHandler(std::shared_ptr<ClientDatabase> client_database, std::string message) -> void
{
    (void)message;
    (void)client_database;
    // auto client = client_database->GetClient(0);
    // client->Access([](IClient &client)
    // {
    //     client.Push(std::make_shared<std::string>("pong"));
    // });
}
