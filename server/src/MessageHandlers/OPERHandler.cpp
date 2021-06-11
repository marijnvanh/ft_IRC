#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/OPERHandler.h"

#define PARAM_USERNAME 0
#define PARAM_PASSWORD 1

OPERHandler::OPERHandler(IServerConfig *server_config,
	IClientDatabase *client_database) :
    CommandHandler(server_config, client_database, "OPER", 2)
{}

OPERHandler::~OPERHandler()
{}

auto OPERHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    auto params = message.GetParams();

	if (!GetOriginalSender(&client, message))
		return ;

	if (client->IsServer()) {
		return ;
	}

	auto administrators = server_config_->GetAdministrators();
    auto admin = administrators.find(params[PARAM_USERNAME]);    
	if (admin == administrators.end() ||
		admin->second != params[PARAM_PASSWORD])
	{
        client->Push(GetErrorMessage(server_config_->GetName(),client->GetPrefix(), ERR_PASSWDMISMATCH, "OPER"));
		return ;
	}
	
	auto user = dynamic_cast<IUser*>(client);
	if (user->HasMode(UserMode::UM_OPERATOR))
		return ;

	user->SetMode(UserMode::UM_OPERATOR, true);
	user->Push(":" + user->GetPrefix() + " MODE " + user->GetNickname() + " :+o");
	user->Push(":" + user->GetPrefix() + " " + std::to_string(RPL_YOUREOPER) + " " + user->GetNickname() + " :You are now an IRC operator");
	
	client_database_->BroadcastToLocalServers(":" + user->GetPrefix() +
		" MODE " + user->GetNickname() + " :+o", message.GetClientUUID());
}

auto OPERHandler::GetOriginalSender(IClient **client, IMessage &message) -> bool
{
	if ((*client)->GetType() == IClient::Type::kLocalServer)
	{
        auto prefix = message.GetPrefix();

		if (!prefix)
		{
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "OPER No nickname given"));
			return (false);
		}

		auto original_client = client_database_->GetClient(*prefix);
		if (!original_client)
		{
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "OPER Could not resolve client " + *prefix));
			return (false);
		}
		*client = *original_client;
	}
	return (true);
}
