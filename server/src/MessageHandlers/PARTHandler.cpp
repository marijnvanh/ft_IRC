#include "Numerics.h"
#include "Utilities.h"
#include "ChannelDatabase.h"
#include "MessageHandlers/PARTHandler.h"

#define CHANNEL_NAME_PARAM 0

PARTHandler::PARTHandler(IClientDatabase *client_database, IChannelDatabase *channel_database)
	: client_database_(client_database), channel_database_(channel_database), logger("PARTHandler")
{}

PARTHandler::~PARTHandler()
{}

static auto StartPartParsing(const std::string& unsplit_channel_names,
	IClient* client, IChannelDatabase *channel_database)
{
	auto channel_names = split(unsplit_channel_names, ",");

	for (auto channel_name : channel_names)
	{
		auto channel = channel_database->GetChannel(channel_name);

		if (!channel)
		{
			client->Push(std::to_string(ERR_NOSUCHCHANNEL) + " :" + channel_name);
			continue;
		}

		if ((*channel)->RemoveUser(client->GetUUID()))
		{
			client->Push("successfully left channel " + channel_name);
			(*channel)->PushToLocal(":" + client->GetNickname() + " :Left channel <" + channel_name + ">");

			// TODO: Send message to all connected server instances.
		}
		else
		{
			client->Push(std::to_string(ERR_NOTONCHANNEL) + " :" + channel_name);			
		}		
	}
}

auto PARTHandler::Handle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	// Handle unregistered client.
	if (client->GetState() == IClient::kUnRegistered)
	{
		client->Push(std::to_string(ERR_NOTREGISTERED));
		return;
	}	
	// Handle not enough parameters.
	if (params.size() == 0)
	{
		client->Push(std::to_string(ERR_NEEDMOREPARAMS));
		return;
	}

	StartPartParsing(params[CHANNEL_NAME_PARAM], client, channel_database_);
}
