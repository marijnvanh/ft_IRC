#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/OPERHandler.h"

#define PARAM_USERNAME 0
#define PARAM_PASSWORD 1

OPERHandler::OPERHandler(IServerConfig *server_config,
	IClientDatabase *client_database) :
    server_config_(server_config),
    client_database_(client_database),
    logger("OPERHandler")
{}

OPERHandler::~OPERHandler()
{}

auto OPERHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    auto params = message.GetParams();

    if (client->GetType() == IClient::Type::kUnRegistered)
    {
        client->Push(GetErrorMessage(ERR_NOTREGISTERED));
        return ;
    }
	if (params.size() < 2)
	{
        client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS, "OPER"));
        return ;		
	}
	if (client->GetType() == IClient::Type::kLocalServer)
	{
        client->Push(GetErrorMessage(ERR_NOOPERHOST, "OPER"));
		return ;
	}

	auto administrators = server_config_->GetAdministrators();
    auto admin = administrators.find(params[PARAM_USERNAME]);
    
	if (admin == administrators.end() ||
		admin->second != params[PARAM_PASSWORD])
	{
        client->Push(GetErrorMessage(ERR_PASSWDMISMATCH, "OPER"));
		return ;
	}
	
	auto user = dynamic_cast<IUser*>(client);
	user->SetMode(UserMode::UM_OPERATOR, true);
    user->Push(std::to_string(RPL_YOUREOPER) + " :You are now an IRC operator");
}
