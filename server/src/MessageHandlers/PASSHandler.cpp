#include "MessageHandlers/PASSHandler.h"
#include "Numerics.h"
#include "Utilities.h"

PASSHandler::PASSHandler(IServerConfig *server_config, IClientDatabase *client_database)
    : server_config_(server_config),
    client_database_(client_database),
    logger("PASSHandler")
{}

PASSHandler::~PASSHandler()
{}

auto PASSHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    
    if (client->GetType() != IClient::Type::kUnRegistered)
    {
        client->Push(GetErrorMessage(server_config_->GetName(), ERR_ALREADYREGISTERED));
        return ;
    }

    logger.Log(LogLevel::DEBUG, "Setting Password");
    
    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Push(GetErrorMessage(server_config_->GetName(), ERR_NEEDMOREPARAMS, "PASS"));
        return ;
    }
    //TODO Parse password
    client->SetPassword(params.front());
}
