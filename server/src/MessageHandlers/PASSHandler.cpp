#include "MessageHandlers/PASSHandler.h"
#include "Numerics.h"

auto PASSHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

    if (client->GetState() != IClient::State::kUnRegistered)
    {
        client->Push(std::to_string(ERR_ALREADYREGISTERED));
        return ;
    }
    
    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Push(std::to_string(ERR_NEEDMOREPARAMS));
        return ;
    }
    //TODO Parse password
    client->SetPassword(params.front());
}
