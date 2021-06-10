#include "Config.h"
#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/VERSIONHandler.h"

#define PARAM_TARGET 0

VERSIONHandler::VERSIONHandler(IServerConfig *server_config, IClientDatabase *client_database) :
    CommandHandler(server_config, client_database, "VERSION", 0, true)
{}

VERSIONHandler::~VERSIONHandler()
{}

auto VERSIONHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    auto params = message.GetParams();

	if (!GetOriginalSender(&client, message))
		return ;

	if (params.size() < 1 || params[PARAM_TARGET] == server_config_->GetName())
	{
    	client->Push(std::to_string(RPL_VERSION) + " :" + PROJECT_VER);
		return ;
	}

	auto target_name = params[PARAM_TARGET];
	auto target = client_database_->GetClient(target_name);
	if (!target)
	{
		client->Push(GetErrorMessage(client->GetPrefix(), ERR_NOSUCHSERVER, target_name));
		return ;
	}
	(*target)->Push(client->GetPrefix() + " VERSION " + target_name);
}

auto VERSIONHandler::GetOriginalSender(IClient **client, IMessage &message) -> bool
{
	if ((*client)->GetType() == IClient::Type::kLocalServer)
	{
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "VERSION No nickname given"));
            return (false);
        }
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "VERSION No such nick: " + *remote_client_nickname));
            return (false);
        }
        *client = *remote_client;
	}
	return (true);
}
