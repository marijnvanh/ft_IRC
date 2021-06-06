#include "MessageHandlers/USERHandler.h"
#include "Numerics.h"
#include "Utilities.h"

#define USERNAME_PARAM 0
#define REALNAME_PARAM 3

USERHandler::USERHandler(IServerConfig *server_config, IClientDatabase *client_database) :
    CommandHandler(server_config, client_database, "USER", 4, true)
{}

USERHandler::~USERHandler()
{}

auto USERHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    auto params = message.GetParams();

	// TOOD: Could we get this from a remote user?
    if (client->GetType() == IClient::Type::kLocalServer)
    {
        client->Push("ERROR: USER command not supported from server");
    }
    
    HandleUSERFromUser(message);
}

auto USERHandler::HandleUSERFromUser(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

    if (client->GetType() != IClient::Type::kUnRegistered)
    {
        client->Push(GetErrorMessage(server_config_->GetName(), ERR_ALREADYREGISTERED));
        return ;
    }

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
