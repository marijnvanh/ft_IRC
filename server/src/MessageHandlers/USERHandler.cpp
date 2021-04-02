#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"


#define USERNAME_PARAM 0
#define REALNAME_PARAM 3

//TODO
static auto HandleUSERFromServer(IClientDatabase *client_database, IMessage &message) -> void
{
    //TODO validate user name and realname
    // auto new_username = message.GetParams()[USERNAME_PARAM];
    // auto new_realname = message.GetParams()[REALNAME_PARAM];
    (void)message;
    (void)client_database;
}

static auto HandleUSERFromUser(IServerConfig *server_config, IClientDatabase *client_database, IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

    if (client->GetState() != IClient::State::kUnRegistered)
    {
        client->Push(std::to_string(ERR_ALREADYREGISTERED));
        return ;
    }

    //TODO validate user name and realname
    auto new_username = message.GetParams()[USERNAME_PARAM];
    auto new_realname = message.GetParams()[REALNAME_PARAM];
    client->SetUsername(new_username);
    client->SetRealname(new_realname);
    
    try {
        client = client_database->RegisterLocalUser(client->GetUUID());
        auto welcome_message = ":" + server_config->GetName() + " 001 " + 
            client->GetNickname() + " :Welcome to " + server_config->GetDescription();
        client->Push(welcome_message);
    } catch (IClientDatabase::UnableToRegister &ex) {
        ;
    }
}

auto USERHandler(IServerConfig *server_config,
    IClientDatabase *client_database,
    IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

    auto params = message.GetParams();
    if (params.size() < 4)
    {
        client->Push(std::to_string(ERR_NEEDMOREPARAMS));
        return ;
    }

    if (client->GetType() == IClient::Type::kServer)
        HandleUSERFromServer(client_database, message);
    else
        HandleUSERFromUser(server_config, client_database, message);
}
