#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/MODEHandler.h"

#define TARGET_IDENTIFIER			0
#define MODE_CHANGES				1
#define CHANNEL_TARGET_IDENTIFIER	2

MODEHandler::MODEHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database) :
	CommandHandler(server_config, client_database, "MODE", 1),
    channel_database_(channel_database)
{}

MODEHandler::~MODEHandler()
{}

auto MODEHandler::HandleChannelTopicSet(IChannel *channel,
	std::optional<std::string> param, bool set) -> void
{
	channel->SetMode(ChannelMode::CM_TOPIC, set);

	// Don't try anything if we're trying to set, but don't have a parameter.
	if (set && param)
		channel->SetTopic(*param);
}

auto MODEHandler::HandleChannelKeySet(IChannel *channel,
	std::optional<std::string> param, bool set) -> void
{
	// Don't try anything if we're trying to set, but don't have a parameter.
	if (set && !param)
		return ;

	channel->SetMode(ChannelMode::CM_KEY, set);
	if (set)
		channel->SetKey(*param);
}

auto MODEHandler::HandleChannelOperatorSet(IUser *user, IChannel *channel,
	std::optional<std::string> param, bool set) -> void
{
	if (!param)
		return ;

	auto other = client_database_->GetClient(*param);
	if (!other)
	{
		user->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHNICK, *param));
		return ;
	}
	
	if (set)
		channel->AddOperator(user->GetUUID());
	else
		channel->RemoveOperator(user->GetUUID());
}

auto MODEHandler::SafeHandle(IMessage &message) -> void
{
    auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    
    if (client->GetType() == IClient::Type::kLocalServer)
    {
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(GetErrorMessage(server_config_->GetName(), ERR_NONICKNAMEGIVEN));
            return ;
        }
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHNICK , *remote_client_nickname));
            return ;
        }
        client = *remote_client;
    }

	if (params[TARGET_IDENTIFIER].at(0) == '#')
	{
		HandleMODEChannel(dynamic_cast<IUser*>(client), params);
		return ;
	}

	HandleMODEUser(dynamic_cast<IUser*>(client), message);
}

auto MODEHandler::HandleMODEUser(IUser *user, IMessage &message) -> void
{
	auto params = message.GetParams();

	if (params[TARGET_IDENTIFIER] != user->GetNickname())
	{
        user->Push(GetErrorMessage(server_config_->GetName(), ERR_USERSDONTMATCH));
		return ;
	}

	if (params.size() < 2)
	{
		user->Push(std::to_string(RPL_UMODEIS) + " " +
			user->GetNickname() + " :" + FormatMode(user->GetMode()));
		return ;
	}

	auto mode = params[MODE_CHANGES];
	auto set = mode.at(0) == '+';
	if (mode[0] != '+' && mode[0] != '-')
	{
		user->Push(GetErrorMessage(server_config_->GetName(), ERR_UMODEUNKNOWNFLAG, std::string(1, mode[0])));
		return ;
	}

	for (size_t i = 1; i < mode.size(); ++i)
	{
		// Ignore operator mode change on user if called on self.
		if (mode[i] == 'o')
			continue ;
		
		if (mode[i] == 's' || mode[i] == 'w' || mode[i] == 'i')
		{
			user->SetMode(static_cast<UserMode>(mode[i] - 65), set);
			continue ;
		}
		if (mode[i] == '+' || mode[i] == '-')
		{
			set = mode[i] == '+';
		}
		user->Push(GetErrorMessage(server_config_->GetName(), ERR_UMODEUNKNOWNFLAG, std::string(1, mode[i])));
		break ;
	}

	std::string prefix = "";
	if (message.GetPrefix())
		prefix = ":" + *(message.GetPrefix()) + " ";

	auto msg = prefix +
		"MODE " + user->GetNickname() +
		" :" + FormatMode(user->GetMode());

	std::optional<IRC::UUID> uuid_opt = std::nullopt;
	if (user->GetType() == IClient::Type::kLocalUser)
		user->Push(msg);
	else
		uuid_opt = std::make_optional<IRC::UUID>(user->GetLocalServer()->GetUUID());

	client_database_->BroadcastToLocalServers(msg, uuid_opt);
}

auto MODEHandler::HandleMODEChannel(IUser *user,
		std::vector<std::string> &params) -> void
{
	auto channel = channel_database_->GetChannel(params[TARGET_IDENTIFIER]);
	if (!channel)
	{
		user->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHCHANNEL, params[TARGET_IDENTIFIER]));
		return ;
	}

	if (!(*channel)->HasUser(user->GetUUID()))
	{
		user->Push(GetErrorMessage(server_config_->GetName(), ERR_NOTONCHANNEL, (*channel)->GetName()));
		return ;		
	}

	if (params.size() < 2)
	{
		user->Push(":" + server_config_->GetName() + " " +
			std::to_string(RPL_CHANNELMODEIS) + " " +
			user->GetNickname() + " " +
			(*channel)->GetName() +	" " + FormatMode((*channel)->GetMode()));
		return ;
	}

	if (!(*channel)->HasOperator(user->GetUUID()))
	{
		user->Push(GetErrorMessage(server_config_->GetName(), ERR_CHANOPRIVSNEEDED, params[TARGET_IDENTIFIER]));
		return ;
	}

	bool set = true;
	uint32_t param_index = 2;
	auto mode = params[MODE_CHANGES];
	if (mode.at(0) != '+' && mode.at(0) != '-')
	{
		return;
	}
	
	for (auto it = mode.begin(); it != mode.end(); ++it)
	{
		unsigned char letter = *it;
		if (letter == '+' || letter == '-')
		{
			set = (letter == '+');
			continue;
		}

		std::optional<std::string> current_param = std::nullopt;
		if (param_index < params.size())
			current_param = std::make_optional<std::string>(params[param_index]);

		if (letter == 'k')
		{
			HandleChannelKeySet(*channel, current_param, set);
			if (set)
				param_index++;
		}
		else if (letter == 't')
		{
			HandleChannelTopicSet(*channel, current_param, set);
			if (set)
				param_index++;
		}
		else if (letter == 'o')
			HandleChannelOperatorSet(user, *channel, current_param, set);
	}

	std::string mode_message = "MODE " + (*channel)->GetName() +
		" " + params[MODE_CHANGES];
	(*channel)->PushToLocal(mode_message, std::optional<IRC::UUID>(user->GetUUID()));
}


auto MODEHandler::FormatMode(std::bitset<64> mode) -> std::string
{
	std::string result("+");

	for (unsigned char i = 0; i < 64; ++i)
	{
		if (mode[i])
		{
			result += i + 65;
		}
	}

	return (result);
}
