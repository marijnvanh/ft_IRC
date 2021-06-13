#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/PONGHandler.h"

#define PARAM_ORIGIN 0
#define PARAM_TARGET 1

PONGHandler::PONGHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "PONG", 0, true)
{}

PONGHandler::~PONGHandler()
{}

auto PONGHandler::HandleForThisServer(IClient *client) -> void
{
	client->SetRespondedToLastPing(true);
}

auto PONGHandler::HandleForTarget(IClient *client, std::string &target_name,
	IMessage &message) -> void
{
	auto params = message.GetParams();

	auto target = client_database_->GetClient(target_name);
	if (!target) {
		client->Push(GetErrorMessage(server_config_->GetName(),
			client->GetPrefix(), ERR_NOSUCHSERVER, target_name));
		return ;
	}

	auto from = client_database_->GetClient(*(message.GetPrefix()));
	if (!from) {
		client->Push(GetErrorMessage(server_config_->GetName(),
			client->GetPrefix(), ERR_NOSUCHSERVER, *(message.GetPrefix())));		
		return ;
	}

	/* Should never happen, but protects against infinite PING-PONG loops */
	if ((*target)->GetUUID() == (*from)->GetUUID())
		return ;

	(*target)->Push(":" + (*from)->GetPrefix() + " PONG " +
		params[PARAM_ORIGIN] + " :" + params[PARAM_TARGET]);
}

auto PONGHandler::SafeHandle(IMessage &message) -> void
{
	auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();

	if (params.size() < 1) {
		if (client->IsUser())
			client->Push(GetErrorMessage(server_config_->GetName(),
				client->GetPrefix(), ERR_NOORIGIN, client->GetPrefix()));
		return ;
	}

	if (!client->IsServer() && params.size() < 2) {
		client->Push(GetErrorMessage(server_config_->GetName(),
			client->GetPrefix(), ERR_NEEDMOREPARAMS));
		return ;
	}

	/* Check if forwarding is needed .*/
	if (params.size() == 2 && client->IsServer()) {
		if (!message.GetPrefix()) {
			client->Push(GetErrorMessage(server_config_->GetName(),
				client->GetPrefix(), ERR_NEEDMOREPARAMS));
			return ;
		}
		
		auto target_name = params[PARAM_TARGET];
		if (target_name != server_config_->GetName()) {
			HandleForTarget(client, target_name, message);
			return ;
		}
	}

	/* If we get here, the PONG message is most likely meant for us. */
	HandleForThisServer(client);
}
