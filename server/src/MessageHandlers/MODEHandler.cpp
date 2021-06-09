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

auto MODEHandler::HandleChannelOperatorSet(IClient *client, IChannel *channel,
	std::optional<std::string> param, bool set) -> bool
{
	if (!param)
		return (false);

	auto target_user = client_database_->GetUser(*param);
	if (!target_user)
	{
		client->Push(GetErrorMessage(server_config_->GetName(),
			ERR_NOSUCHNICK, *param));
		return (false);
	}

	/* No error necessary if the target client is not on the channel. */
	if (!channel->HasUser((*target_user)->GetUUID()))
		return (false);
	
	if (set)
		channel->AddOperator((*target_user)->GetUUID());
	else
		channel->RemoveOperator((*target_user)->GetUUID());
	return (true);
}

auto MODEHandler::SafeHandle(IMessage &message) -> void
{
    auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	if (!GetOriginalSender(&client, message))
		return ;

	if (params[TARGET_IDENTIFIER].at(0) == '#')
	{
		HandleMODEChannel(client, params);
		return ;
	}

	HandleMODEUser(client, message);
}

auto MODEHandler::HandleMODEUser_PreCheck(IClient *origin,
	IMessage &message, IUser **target) -> bool
{
	auto params = message.GetParams();
	auto target_opt = client_database_->GetUser(params[TARGET_IDENTIFIER]);

	/* Target user not found, abort. */
	if (!target_opt) {
		origin->Push(GetErrorMessage(origin->GetPrefix(), ERR_NOSUCHNICK,
			params[TARGET_IDENTIFIER]));
		return (false);
	}

	*target = *target_opt;
	/* Check if origin client is allowed to request or change the mode. */
	if (origin->IsUser() && origin->GetUUID() != (*target)->GetUUID()) {
		origin->Push(GetErrorMessage(origin->GetPrefix(), ERR_USERSDONTMATCH,
			origin->GetNickname()));
		return (false);
	}

	if (params.size() == 1)
	{
		origin->Push(std::to_string(RPL_UMODEIS) + " " +
			(*target)->GetNickname() + " :" + FormatMode((*target)->GetMode()));
		return (false);
	}

	auto mode = params[MODE_CHANGES];
	if (mode[0] != '+' && mode[0] != '-')
	{
		origin->Push(GetErrorMessage(origin->GetPrefix(),
			ERR_UMODEUNKNOWNFLAG, std::string(1, mode[0])));
		return (false);
	}
	return (true);
}

auto MODEHandler::HandleMODEUser(IClient *origin, IMessage &message) -> void
{
	IUser *target = NULL;
	auto params = message.GetParams();

	/* Pre-check for errors and mode request (non-changing). */
	if (!HandleMODEUser_PreCheck(origin, message, &target))
		return ;

	auto mode = params[MODE_CHANGES];
	auto set = mode.at(0) == '+';
	/* The resulting MODE changes after parsing all given flags. */
	auto mode_result = std::string(1, mode.at(0));
	/* Allow admin changes when the original sender is not the target. */
	auto allow_admin_changes = (message.GetClientUUID() != origin->GetUUID());

	for (auto it = mode.begin(); it != mode.end(); ++it) {
		char change = '\0';
		unsigned char letter = *it;

		switch (letter) {
		case '+': case '-':
		{
			auto new_set = letter == '+';
			/* MODE set change, append to mode_result. */
			if (set != new_set) {
				if (mode_result.back() == '+' || mode_result.back() == '-')
					mode_result.pop_back();
				mode_result.push_back(letter);
				set = new_set;
			}
			continue ;
		}
		case 'o':
			/* Operator state only unsettable by client. */
			if (!set || allow_admin_changes) {
				change = 'o';
			} else {
				origin->Push(GetErrorMessage(origin->GetPrefix(),
					ERR_NOPRIVILEGES, "MODE"));
			}
			break ;
		case 's': case 'w': case 'i':
				change = letter;
			break ;
		default:
			if (origin->GetType() == IClient::Type::kLocalUser) {
				origin->Push(GetErrorMessage(origin->GetPrefix(),
					ERR_UMODEUNKNOWNFLAG, std::string(1, letter)));
			}
			break ;
		}

		if (!change)
			continue ;

		target->SetMode(static_cast<UserMode>(change - 65), set);
		mode_result.push_back(change);
	}

	/* Check if there are mode changes. */
	if (mode_result.size() > 1) {
		if (mode_result.back() == '+' || mode_result.back() == '-')
			mode_result.pop_back();

		auto mode_msg = origin->GetPrefix() + " MODE " +
			target->GetNickname() + " :" + mode_result;

		std::optional<IRC::UUID> uuid_opt = std::nullopt;
		if (origin->GetType() == IClient::Type::kLocalUser)
			origin->Push(mode_msg);
		else
			uuid_opt = origin->GetLocalServer()->GetUUID();

		client_database_->BroadcastToLocalServers(mode_msg, uuid_opt);
	}
}

auto MODEHandler::HandleMODEChannel(IClient *origin,
		std::vector<std::string> &params) -> void
{
	auto channel = channel_database_->GetChannel(params[TARGET_IDENTIFIER]);

	if (!channel)
	{
		origin->Push(GetErrorMessage(server_config_->GetName(),
			ERR_NOSUCHCHANNEL, params[TARGET_IDENTIFIER]));
		return ;
	}

	if (params.size() <= 1)
	{
		origin->Push(":" + origin->GetPrefix() + " " +
			std::to_string(RPL_CHANNELMODEIS) + " " +
			origin->GetPrefix() + " " +
			(*channel)->GetName() +	" " + FormatMode((*channel)->GetMode()));
		return ;
	}

	if (!origin->IsServer() && !(*channel)->HasUser(origin->GetUUID()))
	{
		origin->Push(GetErrorMessage(origin->GetPrefix(),
			ERR_NOTONCHANNEL, (*channel)->GetName()));
		return ;
	}

	if (!origin->IsServer() && !(*channel)->HasOperator(origin->GetUUID()))
	{
		origin->Push(GetErrorMessage(server_config_->GetName(),
			ERR_CHANOPRIVSNEEDED, params[TARGET_IDENTIFIER]));
		return ;
	}

	auto mode = params[MODE_CHANGES];
	auto set = mode.at(0) == '+';
	/* The resulting MODE changes after parsing all given flags. */
	auto mode_result = std::string(1, mode.at(0));
	/* The result arguments after parsing all given flags. */
	auto args_result = std::string();
	/* Index of the parameter for the current mode change. */
	uint32_t param_index = 2;
	
	for (auto it = mode.begin(); it != mode.end(); ++it)
	{
		unsigned char letter = *it;
		std::optional<std::string> current_param = std::nullopt;
		if (param_index < params.size())
			current_param = std::make_optional<std::string>(params[param_index]);

		switch (letter) {
		case '+': case '-':
		{
			auto new_set = letter == '+';
			/* MODE set change, append to mode_result. */
			if (set != new_set) {
				if (mode_result.back() == '+' || mode_result.back() == '-')
					mode_result.pop_back();
				mode_result.push_back(letter);
				set = new_set;
			}
			continue ;
		}
		case 'k':
			HandleChannelKeySet(*channel, current_param, set);
			if (set && current_param)
			{
				param_index++;
				args_result += " " + *current_param;
			}
			mode_result.push_back('k');
			break ;
		case 't':
			(*channel)->SetMode(ChannelMode::CM_TOPIC, set);
			mode_result.push_back('t');
			break ;
		case 'o':
			if (HandleChannelOperatorSet(origin, *channel, current_param, set)) {
				mode_result.push_back('o');
				args_result += " " + *current_param;
			}
			break ;
		default:
			if (origin->GetType() == IClient::Type::kLocalUser) {
				origin->Push(GetErrorMessage(origin->GetPrefix(),
					ERR_UMODEUNKNOWNFLAG, std::string(1, letter)));
			}
			break ;
		}
	}

	if (mode_result.size() > 1) {
		if (mode_result.back() == '+' || mode_result.back() == '-')
			mode_result.pop_back();

		std::string mode_message = ":" + origin->GetPrefix() + " MODE " +
			(*channel)->GetName() + " " + mode_result + args_result;
		(*channel)->PushToLocal(mode_message, std::nullopt);

		std::optional<IRC::UUID> ignore_uuid = std::nullopt;
		if (origin->GetType() == IClient::Type::kRemoteUser || origin->IsServer())
			ignore_uuid = std::make_optional<IRC::UUID>(origin->GetLocalServer()->GetUUID());
		client_database_->BroadcastToLocalServers(mode_message, ignore_uuid);
	}
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

auto MODEHandler::GetOriginalSender(IClient **client, IMessage &message) -> bool
{
	if ((*client)->GetType() == IClient::Type::kLocalServer)
	{
        auto remote_sender = message.GetPrefix();

        if (remote_sender == std::nullopt) {
            return (true);
        }
        auto remote_client = client_database_->GetClient(*remote_sender);
        if (remote_client == std::nullopt)
        {
            (*client)->Push(GetErrorMessage(server_config_->GetName(), ERR_NOSUCHNICK , *remote_sender));
            return (false);
        }
        *client = *remote_client;
	}
	return (true);
}
