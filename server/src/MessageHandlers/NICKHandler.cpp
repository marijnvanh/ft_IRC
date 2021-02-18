#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"

auto NICKHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    //TODO validate if it comes from server or client
    (void)client_database;
    (void)message;

    // if (client->Take()->GetState() != IClient::State::kUnRegistered)
    // {
    //     client->Take()->Push(std::to_string(ERR_ALREADYREGISTERED));
    //     return ;
    // }
    
    // auto params = message.GetParams();
    // if (params.size() == 0)
    // {
    //     client->Take()->Push(std::to_string(ERR_NEEDMOREPARAMS));
    //     return ;
    // }
    // //TODO Parse password
    // client->Take()->SetPassword(params.front());
}
