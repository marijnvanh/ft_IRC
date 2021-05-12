#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/TOPICHandler.h"

#define PARAM_CHANNEL_NAME 0
#define PARAM_TOPIC_MESSAGE 1

TOPICHandler::TOPICHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database) :
	CommandHandler(server_config, client_database, "TOPIC", 1),
	channel_database_(channel_database)
{}

TOPICHandler::~TOPICHandler()
{}

auto TOPICHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
	auto params = message.GetParams();

	if (!GetCorrectSender(&client, message))
		return ;

	auto channel = channel_database_->GetChannel(params[PARAM_CHANNEL_NAME]);
	if (!channel || !(*channel)->HasUser(client->GetUUID()))
	{
		client->Push(GetErrorMessage(server_config_->GetName(), ERR_NOTONCHANNEL, params[PARAM_CHANNEL_NAME]));
		return ;
	}

	if (!(*channel)->HasMode(ChannelMode::CM_TOPIC))
	{
		client->Push(std::to_string(RPL_NOTOPIC) + (*channel)->GetName() + " :No topic is set");
		return ;
	}

	// If there's no second parameter, there's just an incoming request for the channel's topic.
	if (params.size() < 2)
	{
		client->Push(std::to_string(RPL_TOPIC) + (*channel)->GetName() + " :" + (*channel)->GetTopic());
		return ;
	}

	if (!(*channel)->HasOperator(client->GetUUID()))
	{
		client->Push(GetErrorMessage(server_config_->GetName(), ERR_CHANOPRIVSNEEDED, params[PARAM_CHANNEL_NAME]));		
		return ;
	}

	(*channel)->SetTopic(params[PARAM_TOPIC_MESSAGE]);
	(*channel)->PushToLocal("TOPIC " + (*channel)->GetName() + " :" + (*channel)->GetTopic(), std::nullopt);
}

auto TOPICHandler::GetCorrectSender(IClient **client, IMessage &message) -> bool
{
	if ((*client)->GetType() == IClient::Type::kLocalServer)
	{
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            (*client)->Push(GetErrorMessage(server_config_->GetName(), ERR_NONICKNAMEGIVEN));
            return (false);
        }
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            (*client)->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHNICK , *remote_client_nickname));
            return (false);
        }
        *client = *remote_client;
	}
	return (true);
}