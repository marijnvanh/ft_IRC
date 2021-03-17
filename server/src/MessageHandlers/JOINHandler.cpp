#include "Numerics.h"
#include "Utilities.h"
#include "ChannelDatabase.h"
#include "MessageHandlers/JOINHandler.h"

#define CHANNEL_NAME_PARAM 0
#define CHANNEL_KEYS_PARAM 1

static auto TryAddUserToChannel(IChannel* channel, const std::string key, IUser* user) -> void
{
	// TODO: Check channel modes to see if user has enough privilege.
	if (channel->GetKey() != key)
	{
		user->Push(std::to_string(ERR_BADCHANNELKEY));
		return;
	}

	channel->AddUser(dynamic_cast<IUser*>(user));
	
	user->Push(std::to_string(RPL_TOPIC));
}

/*         ERR_BANNEDFROMCHAN
           ERR_INVITEONLYCHAN              ERR_BADCHANNELKEY
           ERR_CHANNELISFULL               ERR_BADCHANMASK
           ERR_NOSUCHCHANNEL               ERR_TOOMANYCHANNELS
           RPL_TOPIC*/
auto JOINHandler(IClientDatabase *client_database,
	IChannelDatabase *channel_database, IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database->GetClient(message.GetClientUUID()));

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

	// TODO: Handle server message.

	auto channel_names = split(params[CHANNEL_NAME_PARAM], ",");
	
	std::vector<std::string> channel_keys;
	if (params.size() >= 2)
	{
		channel_keys = split(params[CHANNEL_KEYS_PARAM], ",");
	}

	// TODO: F*ck me, this is ugly.
	auto it = channel_names.begin();
	for (; it != channel_names.end(); ++it)
	{
		if (it->at(0) != '#' && it->at(0) != '&')
		{
			client->Push(std::to_string(ERR_NOSUCHCHANNEL));
			continue;
		}

		std::string key;
		if (channel_keys.size() > (unsigned)(it - channel_names.begin()))
		{
			key.assign(channel_keys[it - channel_names.begin()]);
		}

		auto current_channel = channel_database->GetChannel(*it);

		if (!current_channel)
		{
			current_channel = channel_database->CreateChannel(*it, key, ChannelType::kLocal, ChannelMode::None);
		}

		// Assuming current_channel will never be null here. Should we do a safety null check?
		TryAddUserToChannel(*current_channel, key, dynamic_cast<IUser*>(client));
	}
}
