#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/PARTHandler.h"

#define CHANNEL_NAME_PARAM 0
#define PART_MESSAGE_PARAM 1

PARTHandler::PARTHandler(IServerConfig *server_config,
	IClientDatabase *client_database, IChannelDatabase *channel_database) :
	CommandHandler(server_config, client_database, "PART", 1),
	channel_database_(channel_database)
{}

PARTHandler::~PARTHandler()
{}

auto PARTHandler::SafeHandle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	if (!GetOriginalSender(&client, message))
		return ;

	StartPartParsing(params, client);
}

auto PARTHandler::StartPartParsing(std::vector<std::string> params, IClient* client) -> void
{
	auto channel_names = split(params[CHANNEL_NAME_PARAM], ",");
	std::string part_message(" :" + client->GetNickname() + " left");
	if (params.size() > 1)
	{
		part_message.assign(" :" + params[PART_MESSAGE_PARAM]);
	}

	for (auto channel_name : channel_names)
	{
		auto channel = channel_database_->GetChannel(channel_name);

		if (!channel)
		{
			client->Push(GetErrorMessage(server_config_->GetName(), client->GetPrefix(), ERR_NOSUCHCHANNEL, channel_name));
			continue;
		}

		if ((*channel)->HasUser(client->GetUUID()))
		{
			auto user = dynamic_cast<IUser*>(client);
			user->RemoveChannel(channel_name);
			
			auto part_irc_msg = ":" + client->GetPrefix() + " PART " + channel_name + part_message;
			(*channel)->PushToLocal(part_irc_msg, std::nullopt);
			(*channel)->RemoveUser(client->GetUUID());
		}
		else
		{
			client->Push(GetErrorMessage(server_config_->GetName(), client->GetPrefix(), ERR_NOTONCHANNEL, channel_name));			
		}		
	}

	auto part_irc_msg = ":" + client->GetPrefix() + " PART " + params[CHANNEL_NAME_PARAM] + part_message;
	if (client->GetType() == IClient::Type::kLocalServer)
		client_database_->BroadcastToLocalServers(part_irc_msg, client->GetUUID());
	else
		client_database_->BroadcastToLocalServers(part_irc_msg, std::nullopt);
}

auto PARTHandler::GetOriginalSender(IClient **client, IMessage &message) -> bool
{
	if ((*client)->GetType() == IClient::Type::kLocalServer)
	{
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "PART No nickname given"));
            return (false);
        }
        auto remote_client = client_database_->GetUser(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
			(*client)->Push(FormatERRORMessage((*client)->GetPrefix(), "PART No suck nick " + *remote_client_nickname));
            return (false);
        }
        *client = *remote_client;
	}
	return (true);
}
