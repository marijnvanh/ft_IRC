#include "MessageHandlers/PASSHandler.h"
#include "Numerics.h"
#include "Utilities.h"

PASSHandler::PASSHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "PASS", 1, true)
{}

PASSHandler::~PASSHandler()
{}

auto PASSHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    
    if (client->GetType() != IClient::Type::kUnRegistered)
    {
        if (client->IsServer()) {
            client->Push(FormatERRORMessage(client->GetPrefix(), "PASS Already registered"));
        }
        else {
            client->Push(GetErrorMessage(server_config_->GetName(), client->GetPrefix(), ERR_ALREADYREGISTERED));
        }
        return ;
    }

    logger_.Log(LogLevel::DEBUG, "Setting Password");
    
    auto params = message.GetParams();
    client->SetPassword(params.front());
}
