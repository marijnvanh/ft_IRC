#include "MessageHandlers/USERHandler.h"
#include "Numerics.h"
#include "Utilities.h"

#define USERNAME_PARAM 0
#define REALNAME_PARAM 3

USERHandler::USERHandler(IServerConfig *server_config, IClientDatabase *client_database) :
    server_config_(server_config),
    client_database_(client_database),
    logger("USERHandler")
{}

USERHandler::~USERHandler()
{}

auto USERHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

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
    
    HandleUSERFromUser(message);
}

auto USERHandler::HandleUSERFromUser(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

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
        client = client_database_->RegisterLocalUser(client->GetUUID());
        auto welcome_message = ":" + server_config_->GetName() + " 001 " + 
            client->GetNickname() + " :Welcome to " + server_config_->GetDescription();
        client->Push(welcome_message);
    } catch (IClientDatabase::UnableToRegister &ex) {
        ;
    }
}
