#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/MODEHandler.h"

#define TARGET_IDENTIFIER			0
#define MODE_CHANGES				1
#define CHANNEL_TARGET_IDENTIFIER	2

/*
ERR_NEEDMOREPARAMS RPL_CHANNELMODEIS ERR_NOSUCHNICK*/

MODEHandler::MODEHandler(IClientDatabase *client_database, IChannelDatabase *channel_database) :
    logger("MODEHandler"),
    client_database_(client_database),
    channel_database_(channel_database)
{}

MODEHandler::~MODEHandler()
{}

static auto HandleChannelTopicSet(IUser *user, IChannel *channel,
	std::vector<std::string> params, bool set) -> void
{
	if (params.size() < 3)
	{
		user->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
		return ;
	}

	channel->SetMode(ChannelMode::CM_TOPIC, set);
	if (set)
	{
		channel->SetTopic(params[CHANNEL_TARGET_IDENTIFIER]);
	}	
}

static auto HandleChannelKeySet(IUser *user, IChannel *channel,
	std::vector<std::string> params, bool set) -> void
{
	if (params.size() < 3)
	{
		user->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
		return ;
	}

	channel->SetMode(ChannelMode::CM_KEY, set);
	if (set)
	{
		channel->SetKey(params[CHANNEL_TARGET_IDENTIFIER]);
	}
}

static auto HandleChannelOperatorSet(IUser *user, IChannel *channel,
	IClientDatabase *client_database, std::vector<std::string> params, bool set) -> void
{
	if (params.size() < 3)
	{
		user->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
		return ;
	}

	auto nick = params[CHANNEL_TARGET_IDENTIFIER];
	auto other = client_database->GetClient(nick);
	if (!other)
	{
		user->Push(GetErrorMessage(ERR_NOSUCHNICK, nick));
		return ;
	}
	
	if (set)
	{
		channel->AddOperator(user->GetUUID());
	}
	else
	{
		channel->RemoveOperator(user->GetUUID());
	}	
}

auto MODEHandler::Handle(IMessage &message) -> void
{
    auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

    if (client->GetType() == IClient::Type::kUnRegistered)
    {
        client->Push(GetErrorMessage(ERR_NOTREGISTERED));
        return ;
    }

    if (params.size() == 0)
    {
        client->Push(GetErrorMessage(ERR_NEEDMOREPARAMS));
        return ;
    }
    
    if (client->GetType() == IClient::Type::kLocalServer)
    {
        auto remote_client_nickname = message.GetNickname();
    
        if (remote_client_nickname == std::nullopt)
        {
            client->Push(GetErrorMessage(ERR_NONICKNAMEGIVEN));
            return ;
        }
        //TODO validate nickname
        auto remote_client = client_database_->GetClient(*remote_client_nickname);
        if (remote_client == std::nullopt)
        {
            client->Push(GetErrorMessage(ERR_NOSUCHNICK , *remote_client_nickname));
            return ;
        }
        client = *remote_client;
    }

	if (params[TARGET_IDENTIFIER].at(0) == '#')
	{
		HandleMODEChannel(dynamic_cast<IUser*>(client), params);
		return ;
	}

	HandleMODEUser(dynamic_cast<IUser*>(client), params);
}

auto MODEHandler::HandleMODEUser(IUser *user,
		std::vector<std::string> params) -> void
{
	if (params[TARGET_IDENTIFIER] != user->GetNickname())
	{
        user->Push(GetErrorMessage(ERR_USERSDONTMATCH));
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
		user->Push(GetErrorMessage(ERR_UMODEUNKNOWNFLAG, std::string(1, mode[0])));
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
		user->Push(GetErrorMessage(ERR_UMODEUNKNOWNFLAG, std::string(1, mode[i])));
		break ;
	}

	user->Push("MODE " + user->GetNickname() +
		" :" + FormatMode(user->GetMode()));
}

auto MODEHandler::HandleMODEChannel(IUser *user,
		std::vector<std::string> params) -> void
{
	auto channel = channel_database_->GetChannel(params[TARGET_IDENTIFIER]);
	if (!channel)
	{
		user->Push(GetErrorMessage(ERR_NOSUCHCHANNEL, params[TARGET_IDENTIFIER]));
		return ;
	}

	if (!(*channel)->HasUser(user->GetUUID()))
	{
		user->Push(GetErrorMessage(ERR_NOTONCHANNEL, (*channel)->GetName()));
		return ;		
	}

	if (params.size() < 2)
	{
		user->Push(std::to_string(RPL_CHANNELMODEIS) +
			" " + (*channel)->GetName() + " :" + FormatMode((*channel)->GetMode()));
		return ;
	}

	if (!(*channel)->HasOperator(user->GetUUID()))
	{
		user->Push(GetErrorMessage(ERR_CHANOPRIVSNEEDED, params[TARGET_IDENTIFIER]));
	}

	auto mode = params[MODE_CHANGES];
	auto set = mode.at(0) == '+';
	if (mode[0] != '+' && mode[0] != '-')
	{
		user->Push(GetErrorMessage(ERR_UMODEUNKNOWNFLAG, std::string(1, mode[0])));
		return ;
	}

	for (size_t i = 1; i < mode.size(); ++i)
	{
		if (mode[i] == 'k')
		{
			HandleChannelKeySet(user, *channel, params, set);
		}
		else if (mode[i] == 't')
		{
			HandleChannelTopicSet(user, *channel, params, set);
		}
		else if (mode[i] == 'o')
		{
			HandleChannelOperatorSet(user, *channel, client_database_, params, set);
		}
		else if (mode[i] == '+' || mode[i] == '-')
		{
			set = mode[i] == '+';
		}
		else
		{
			user->Push(GetErrorMessage(ERR_UMODEUNKNOWNFLAG, std::string(1, mode[i])));
			return ;
		}
	}
	std::string mode_message = "MODE " + (*channel)->GetName() +
		" " + params[MODE_CHANGES] + " " + params[CHANNEL_TARGET_IDENTIFIER];
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
