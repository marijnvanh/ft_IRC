#include "MessageHandlers/PASSHandler.h"
#include "Numerics.h"
#include "Utilities.h"

PASSHandler::PASSHandler(IClientDatabase *client_database) : client_database_(client_database), logger("PASSHandler")
{}

PASSHandler::~PASSHandler()
{}

auto PASSHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    
    if (client->GetType() != IClient::Type::kUnRegistered)
    {
        client->Push(GetErrorMessage(ERR_ALREADYREGISTERED));
        return ;
    }

    logger.Log(LogLevel::DEBUG, "Setting Password");
    
    auto params = message.GetParams();
    if (params.size() == 0)
    {
        client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
        return ;
    }
    //TODO Parse password
    client->SetPassword(params.front());
}
