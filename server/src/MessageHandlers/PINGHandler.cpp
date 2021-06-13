#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/PINGHandler.h"

#define PARAM_ORIGIN 0
#define PARAM_TARGET 1

PINGHandler::PINGHandler(IServerConfig *server_config, IClientDatabase *client_database) :
	CommandHandler(server_config, client_database, "PING", 0, true)
{}

PINGHandler::~PINGHandler()
{}

auto PINGHandler::HandleForTarget(IClient *client, IMessage &message) -> bool
{
	std::string origin;
	auto params = message.GetParams();
	auto target_name = params[PARAM_TARGET];

	/* If the target is the current server, we immediately return false. */
	if (target_name == server_config_->GetName())
		return (false);

	/* If the target is not a server, don't continue with handling the PING. */
	auto target = client_database_->GetServer(target_name);
	if (!target) {
		if (client->IsServer()) {
			client->Push(":" + server_config_->GetName() +
				" ERROR :PING target not known.");
		} else {
			client->Push(GetErrorMessage(server_config_->GetName(),
				client->GetPrefix(), ERR_NOSUCHSERVER, target_name));
		}
		return (true);
	}

	/* Make sure the prefix for the redirect message is set correctly */
	if (client->IsServer()) {
		if (!message.GetPrefix()) {
			if (client->IsServer()) {
				client->Push(":" + server_config_->GetName() +
					" ERROR :PING message doesn't have a prefix.");
			} else {
				client->Push(GetErrorMessage(client->GetPrefix(),
					client->GetPrefix(), ERR_NEEDMOREPARAMS, "PING"));
			}
			return (true);
		}
		origin = *(message.GetPrefix());
	} else {
		origin = client->GetPrefix();
	}

	(*target)->Push(":" + origin + " PING " + params[PARAM_ORIGIN] +
		" :" + target_name);

	return (true);
}

auto PINGHandler::HandleForThisServer(IClient* client, IMessage &message) -> void
{
	std::string origin;
	auto params = message.GetParams();

	/* Make sure the origin parameter for the PONG message is set correctly. */
	if (client->IsServer()) {
		if (message.GetPrefix())
			origin = *(message.GetPrefix());
		else
			origin = client->GetPrefix();
	} else
		origin = server_config_->GetName();

	client->Push(":" + server_config_->GetName() +
		" PONG " + origin + " :" + params[PARAM_ORIGIN]);	
}

auto PINGHandler::SafeHandle(IMessage &message) -> void
{
	auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();

	/* No origin specified, push error. */
	if (params.size() < 1) {
		if (client->IsServer()) {
			client->Push(":" + server_config_->GetName() +
				" ERROR :PING no origin given.");
		} else {
			client->Push(GetErrorMessage(server_config_->GetName(),
					client->GetPrefix(), ERR_NOORIGIN, client->GetPrefix()));
		}
		return ;
	}

	/* Target specified, test to see if we need to try and forward */
	if (params.size() > 1 && HandleForTarget(client, message))
		return ;

	HandleForThisServer(client, message);
}
