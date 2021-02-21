#include "MessageHandlers/PASSHandler.h"
#include "Numerics.h"

auto PASSHandler(IMessage &message) -> void
{
    std::shared_ptr<IRC::Mutex<IClient>> client = message.GetClient();

    if (client->Take()->GetState() != IClient::State::kUnRegistered)
    {
        client->Take()->Push(std::to_string(ERR_ALREADYREGISTERED));
        return ;
    }
    
    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Take()->Push(std::to_string(ERR_NEEDMOREPARAMS));
        return ;
    }
    //TODO Parse password
    client->Take()->SetPassword(params.front());
}
