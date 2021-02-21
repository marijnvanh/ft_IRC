#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"
#include "RegisterUser.h"


#define USERNAME_PARAM 0
#define REALNAME_PARAM 3

//TODO
static auto HandleUSERFromServer(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    std::shared_ptr<IRC::Mutex<IClient>> server = message.GetClient();
    (void)server;
    (void)message;
    (void)client_database;
}

static auto HandleUSERFromUser(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    std::shared_ptr<IRC::Mutex<IClient>> client = message.GetClient();

    if (client->Take()->GetState() != IClient::State::kUnRegistered)
    {
        client->Take()->Push(std::to_string(ERR_ALREADYREGISTERED));
        return ;
    }

    //TODO validate user name and realname
    auto new_username = message.GetParams()[USERNAME_PARAM];
    auto new_realname = message.GetParams()[REALNAME_PARAM];

    {
        auto client_handle = client->Take();
        client_handle->SetUsername(new_username);
        client_handle->SetRealname(new_realname);
    }
    RegisterUser(client_database, client);
}

auto USERHandler(std::shared_ptr<IClientDatabase> client_database, IMessage &message) -> void
{
    std::shared_ptr<IRC::Mutex<IClient>> client = message.GetClient();

    auto params = message.GetParams();
    if (params.size() < 4)
    {
        client->Take()->Push(std::to_string(ERR_NEEDMOREPARAMS));
        return ;
    }

    if (client->Take()->GetType() == IClient::Type::kServer)
        HandleUSERFromServer(client_database, message);
    else
        HandleUSERFromUser(client_database, message);
}
