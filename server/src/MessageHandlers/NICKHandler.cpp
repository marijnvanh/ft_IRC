#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"

//INFO Currently handles client side NICK message only
//TODO handle server side message
//TODO validate if it comes from server or client
auto NICKHandler(std::shared_ptr<IClientDatabase> client_database,
    std::shared_ptr<IRC::Mutex<IClient>> client, IMessage &message) -> void
{
    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Take()->Push(std::to_string(ERR_NONICKNAMEGIVEN));
        return ;
    }

    auto nickname = params.front();
    
    //TODO validate nickname

    if (client->Take()->GetNickname() == nickname)
    {
        client->Take()->Push(std::to_string(ERR_NICKCOLLISION));
        return ;
    }
    
    auto client_with_nickname = client_database->Find(nickname);

    if (client_with_nickname)
    {
        client->Take()->Push(std::to_string(ERR_NICKNAMEINUSE));
        return ;
    }
    client->Take()->SetNickname(nickname);
}
