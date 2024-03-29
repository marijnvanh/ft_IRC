#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/CommandHandler.h"

CommandHandler::CommandHandler(IServerConfig* server_config,
	IClientDatabase *client_database,
	std::string command_name,
	uint8_t min_params,
	bool allow_unregistered) :
	min_params_(min_params),
	allow_unregistered_(allow_unregistered),
	logger_(command_name + "Handler"),
	server_config_(server_config),
	client_database_(client_database),
	command_name_(command_name)
{}

auto CommandHandler::Handle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	// Preliminary parameter amount check.
	if (min_params_ && message.GetParams().size() < min_params_)
	{
		if (client->IsServer()) {
			client->Push(FormatERRORMessage(client->GetPrefix(), "Need more params for " + command_name_));
		}
		else {
			client->Push(GetErrorMessage(server_config_->GetName(), client->GetPrefix(), ERR_NEEDMOREPARAMS, command_name_));
		}
		return;
	}

	// Preliminary client registered check.
	if (!allow_unregistered_ && client->GetType() == IClient::Type::kUnRegistered)
	{		
		client->Push(GetErrorMessage(server_config_->GetName(), client->GetPrefix(), ERR_NOTREGISTERED));
		return;
	}

	// Safely handle command after preliminary checks.
	// Additional (situational) checks might be required after this point.
	SafeHandle(message);
}
