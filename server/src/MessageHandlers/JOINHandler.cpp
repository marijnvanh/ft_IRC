#include <map>

#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/JOINHandler.h"

#define CHANNEL_NAME_PARAM 0
#define CHANNEL_KEYS_PARAM 1

JOINHandler::JOINHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database) :
	CommandHandler(server_config, client_database, "JOIN", 1),
	channel_database_(channel_database)
{}
JOINHandler::~JOINHandler()
{}

auto JOINHandler::CreateChannelKeyMap(std::string param_name, std::string param_key) -> const std::map<std::string, std::string>
{
	auto channel_keys = split(param_key, ",");
	auto channel_names = split(param_name, ",");

	std::map<std::string, std::string> nameKeyMap;

	for (size_t i = 0; i < channel_names.size(); ++i)
	{
		if (channel_keys.size() > i)
		{
			nameKeyMap[channel_names[i]] = channel_keys[i];
		}
		else
		{
			nameKeyMap[channel_names[i]] = std::string();
		}		
	}

	return (nameKeyMap);
}

auto JOINHandler::TryAddUserToChannel(IChannel* channel, const std::string key, IUser* user, bool isOp) -> bool
{
	// TODO: Check channel modes to see if user has enough privilege.
	if (channel->HasMode(ChannelMode::CM_KEY) && channel->GetKey() != key)
	{
		user->Push(GetErrorMessage(server_config_->GetName(), ERR_BADCHANNELKEY, channel->GetName()));
		return (false);
	}

	// Disallow users from joining the same channel twice.
	// Undefined behaviour, since a user should never attempt this.
	if (channel->HasUser(user->GetUUID()))
	{
		return (false);
	}

	channel->AddUser(user);
	user->AddChannel(channel);
	std::string join_message = ":" + user->GetNickname() + " JOIN " + channel->GetName();
	channel->PushToLocal(join_message, std::optional<IRC::UUID>(user->GetUUID()));
	if (isOp)
	{
		channel->AddOperator(user->GetUUID());
	}
	
	if (user->GetType() == IClient::Type::kLocalUser)
	{
		user->Push(std::to_string(RPL_TOPIC) + " :" + channel->GetTopic());
		user->Push(std::to_string(RPL_NAMREPLY) + " " + channel->GetName() + " :" + channel->GetUserListAsString());
		user->Push(std::to_string(RPL_ENDOFNAMES) + " " + channel->GetName() + ":End of /NAMES list");
	}
	return (true);
}


auto JOINHandler::StartJoinParsing(const std::vector<std::string> &params,
	IClient* client, IChannelDatabase *channel_database) -> void
{
	auto keys = params.size() >= 2 ? params[CHANNEL_KEYS_PARAM] : std::string();
	auto channel_pairs = CreateChannelKeyMap(params[CHANNEL_NAME_PARAM], keys);

	for (auto kvp : channel_pairs)
	{
		bool channel_created = false;
		// TODO: The length and prefix check should probably be part of the parser(?)
		if (kvp.first.at(0) != '#' || kvp.first.size() >= 50)
		{
			client->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHCHANNEL, kvp.first));
			continue;
		}

		auto current_channel = channel_database->GetChannel(kvp.first);

		if (!current_channel)
		{
			channel_created = true;
			current_channel = channel_database->CreateChannel(kvp.first,
				kvp.second, ChannelType::kLocal);
		}

		TryAddUserToChannel(*current_channel, kvp.second,
			dynamic_cast<IUser*>(client), channel_created);
	}
}

auto JOINHandler::SafeHandle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	// Handle server message.
	if (client->GetType() == IClient::Type::kLocalServer)
	{
        auto remote_client_nickname = message.GetNickname();
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(GetErrorMessage(server_config_->GetName(), ERR_NONICKNAMEGIVEN));
            return ;
        }
        //TODO validate nickname
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHNICK));
            return ;
        }
        client = *remote_client;
	}
	
	StartJoinParsing(params, client, channel_database_);

	/* Broadcast to server */
	auto join_msg = ":" + client->GetNickname() + " JOIN " + params[CHANNEL_NAME_PARAM];
	if (params.size() >= 2)
		join_msg = join_msg + " " + params[CHANNEL_KEYS_PARAM];
	if (client->GetType() == IClient::Type::kRemoteUser)
		client_database_->BroadcastToLocalServers(join_msg, client->GetLocalServer()->GetUUID());
	else
		client_database_->BroadcastToLocalServers(join_msg, std::nullopt);
}
