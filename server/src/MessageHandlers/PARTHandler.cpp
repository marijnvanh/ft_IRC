#include "Numerics.h"
#include "Utilities.h"
#include "ChannelDatabase.h"
#include "MessageHandlers/PARTHandler.h"

#define CHANNEL_NAME_PARAM 0
#define PART_MESSAGE_PARAM 1

PARTHandler::PARTHandler(IClientDatabase *client_database, IChannelDatabase *channel_database)
	: client_database_(client_database), channel_database_(channel_database), logger("PARTHandler")
{}

PARTHandler::~PARTHandler()
{}

static auto StartPartParsing(std::vector<std::string> params,
	IClient* client, IChannelDatabase *channel_database)
{
	auto channel_names = split(params[CHANNEL_NAME_PARAM], ",");
	std::string part_message(":" + client->GetNickname() + " left");

	if (params.size() > 1)
	{
		part_message.assign(":" + params[PART_MESSAGE_PARAM]);
	}

	for (auto channel_name : channel_names)
	{
		auto channel = channel_database->GetChannel(channel_name);

		if (!channel)
		{
			client->Push(GetErrorMessage(ERR_NOSUCHCHANNEL, channel_name));
			continue;
		}

		if ((*channel)->RemoveUser(client->GetUUID()))
		{
			auto user = dynamic_cast<IUser*>(client);
			user->RemoveChannel(channel_name);
			part_message = ":" + client->GetNickname() + " PART " + channel_name + " " + part_message;
			(*channel)->PushToLocal(part_message, std::nullopt);

			// TODO: Send message to all connected server instances.
		}
		else
		{
			client->Push(GetErrorMessage(ERR_NOTONCHANNEL, channel_name));			
		}		
	}
}

auto PARTHandler::Handle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	// Handle unregistered client.
	if (client->GetType() == IClient::Type::kUnRegistered)
	{
		client->Push(GetErrorMessage(ERR_NOTREGISTERED));
		return;
	}	
	// Handle not enough parameters.
	if (params.size() == 0)
	{
		client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS, "PART"));
		return;
	}

	StartPartParsing(params, client, channel_database_);
}
