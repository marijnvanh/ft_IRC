#include "MessageHandlers/NICKHandler.h"
#include "Numerics.h"
#include "Utilities.h"


#define USERNAME_PARAM 0
#define REALNAME_PARAM 3

static auto HandleUSERFromUser(IServerConfig *server_config, IClientDatabase *client_database, IMessage &message) -> void
{
    auto client = *(client_database->GetClient(message.GetClientUUID()));

    if (client->GetState() != IClient::State::kUnRegistered)
    {
        client->Push(GetErrorMessage(ERR_ALREADYREGISTERED));
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
        client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
        return ;
    }

    if (client->GetType() == IClient::Type::kServer)
    {
        client->Push("ERROR: USER command not supported from server");
    }
    
    HandleUSERFromUser(server_config, client_database, message);
}
