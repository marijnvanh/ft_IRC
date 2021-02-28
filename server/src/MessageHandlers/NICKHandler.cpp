#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"

//TODO handle server side message
static auto HandleNICKFromServer(std::shared_ptr<IClientDatabase> client_database,
    std::shared_ptr<IRC::Mutex<IClient>> server, IMessage &message,
    const std::string &new_nickname) -> void
{
    (void)server;
    auto old_nickname = message.GetNickname();

    if (old_nickname == std::nullopt)
    {
        //TODO respond with error, maybe ERR_NONICKNAMEGIVEN ?
        server->Take()->Push(std::to_string(ERR_NONICKNAMEGIVEN));
        return ;
    }

    auto client_with_same_nickname = client_database->Find(new_nickname);
    if (client_with_same_nickname)
    {
        //TODO send kill command to both users with nickname
        //Do after we finish kill command
        return ;
    }

    auto client = client_database->Find(*old_nickname);
    if (client)
        (*client)->Take()->SetNickname(new_nickname);
    else
    {
        ; //TODO What do we do if we do not have that client somehow?
    }
}

static auto HandleNICKFromUser(std::shared_ptr<IClientDatabase> client_database,
    std::shared_ptr<IRC::Mutex<IClient>> client, const std::string &nickname) -> void
{
    auto client_with_nickname = client_database->Find(nickname);

    if (client_with_nickname)
    {
        client->Take()->Push(std::to_string(ERR_NICKNAMEINUSE));
        return ;
    }

    if (client->Take()->GetNickname() == nickname)
    {
        client->Take()->Push(std::to_string(ERR_NICKCOLLISION));
        return ;
    }
    client->Take()->SetNickname(nickname);

    if (client->Take()->GetState() == IClient::State::kRegistered)
        ;//TODO Inform all connected clients that nickname has changed
    else if (client->Take()->GetState() == IClient::State::kUnRegistered)
    {
        try {
            client_database->RegisterLocalUser(client->Take()->GetUUID());
        } catch (IClientDatabase::UnAbleToRegister &ex) {
            ;
        }
    }
}

auto NICKHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    std::shared_ptr<IRC::Mutex<IClient>> client = message.GetClient();

    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Take()->Push(std::to_string(ERR_NONICKNAMEGIVEN));
        return ;
    }

    auto nickname = params.front();
    //TODO validate nickname

    if (client->Take()->GetType() == IClient::Type::kServer)
        HandleNICKFromServer(client_database, client, message, nickname);
    else
        HandleNICKFromUser(client_database, client, nickname);
}
