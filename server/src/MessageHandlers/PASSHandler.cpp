#include "MessageHandlers/PASSHandler.h"
#include "Numerics.h"
#include "Utilities.h"

PASSHandler::PASSHandler(IClientDatabase *client_database) :
	CommandHandler(client_database, "PASS", 1, true)
{}

PASSHandler::~PASSHandler()
{}

auto PASSHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    auto params = message.GetParams();
	
	if (client->GetType() != IClient::Type::kUnRegistered)
	{
		client->Push(GetErrorMessage(ERR_ALREADYREGISTERED));
		return ;
	}

    logger_.Log(LogLevel::DEBUG, "Setting Password");

    //TODO Parse password
    client->SetPassword(params.front());
}
