#include "MessageHandlers/PASSHandler.h"
#include "Numerics.h"

auto PASSHandler(IMessage &message) -> void
{
    std::shared_ptr<IClient> client = message.GetClient();

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
