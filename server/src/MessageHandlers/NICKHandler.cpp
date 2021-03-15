#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"

//TODO handle server side message
static auto HandleNICKFromServer(IClientDatabase *client_database,
    IClient* server, IMessage &message,
    const std::string &new_nickname) -> void
{
    (void)server;
    auto old_nickname = message.GetNickname();

    if (old_nickname == std::nullopt)
    {
        //TODO respond with error, maybe ERR_NONICKNAMEGIVEN ?
        server->Push(std::to_string(ERR_NONICKNAMEGIVEN));
        return ;
    }

    auto client_with_same_nickname = client_database->GetClient(new_nickname);
    if (client_with_same_nickname)
    {
        //TODO send kill command to both users with nickname
        //Do after we finish kill command
        return ;
    }

    auto client = client_database->GetClient(*old_nickname);
    if (client)
        (*client)->SetNickname(new_nickname);
    else
    {
        ; //TODO What do we do if we do not have that client somehow?
    }
}

static auto HandleNICKFromUser(IClientDatabase *client_database,
    IClient* client, const std::string &nickname) -> void
{
    auto client_with_nickname = client_database->GetClient(nickname);

    if (client_with_nickname)
    {
        client->Push(std::to_string(ERR_NICKNAMEINUSE));
        return ;
    }

    if (client->GetNickname() == nickname)
    {
        client->Push(std::to_string(ERR_NICKCOLLISION));
        return ;
    }
    client->SetNickname(nickname);

    if (client->GetState() == IClient::State::kRegistered)
        ;//TODO Inform all connected clients that nickname has changed
    else if (client->GetState() == IClient::State::kUnRegistered)
    {
        try {
            client_database->RegisterLocalUser(client->GetUUID());
        } catch (IClientDatabase::UnableToRegister &ex) {
            ;
        }
    }
}

auto NICKHandler(IClientDatabase *client_database, IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Push(std::to_string(ERR_NONICKNAMEGIVEN));
        return ;
    }

    auto nickname = params.front();
    //TODO validate nickname

    if (client->GetType() == IClient::Type::kServer)
        HandleNICKFromServer(client_database, client, message, nickname);
    else
        HandleNICKFromUser(client_database, client, nickname);
}
