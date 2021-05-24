#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/NJOINHandler.h"

#define CHANNEL_NAME_PARAM			0
#define USERNAME_LIST_PARAM		    1

/*
    Command: NJOIN
   Parameters: <channel> [ "@@" / "@" ] [ "+" ] <nickname>
                         *( "," [ "@@" / "@" ] [ "+" ] <nickname> )

   The prefix "@@" indicates that the user is the "channel creator", the
   character "@" alone indicates a "channel operator", and the character
   '+' indicates that the user has the voice privilege.

   Numeric Replies:

           ERR_NEEDMOREPARAMS              ERR_NOSUCHCHANNEL
           ERR_ALREADYREGISTRED

   Examples:

   :codam.irc.com NJOIN #new_channel :@nick1,+nick2,nick3 ;
*/



NJOINHandler::NJOINHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database) :
	CommandHandler(server_config, client_database, "NJOIN", 2),
    channel_database_(channel_database)
{}

NJOINHandler::~NJOINHandler()
{}

auto NJOINHandler::SafeHandle(IMessage &message) -> void
{
	auto params = message.GetParams();
    auto client = *(client_database_->GetClient(message.GetClientUUID()));

	/* Ignore messages from non server clients. */
	if (client->GetType() != IClient::Type::kLocalServer) {
        logger_.Log(LogLevel::WARNING, "NJOIN from non server ignored");
        return ;
    }

    auto channel = channel_database_->GetChannel(params[CHANNEL_NAME_PARAM]);
    if (!channel) {
        channel = channel_database_->CreateChannel(params[CHANNEL_NAME_PARAM],"", ChannelType::kNetwork);
    }
    
    auto usernames = split(params[USERNAME_LIST_PARAM], ",");
    AddUsersToChannel(*channel, usernames);

	/* Broadcast to local servers */
    auto prefix = message.GetPrefix() ? *(message.GetPrefix()) : server_config_->GetName();
	auto njoin_msg = ":" + prefix + " NJOIN " + params[CHANNEL_NAME_PARAM] +  " :" + params[USERNAME_LIST_PARAM];
	client_database_->BroadcastToLocalServers(njoin_msg, message.GetClientUUID());
}

auto NJOINHandler::AddUsersToChannel(IChannel* channel, std::vector<std::string> &usernames) -> void
{
    for (std::string username : usernames) {
        bool is_operator = false;
        if (username[0] == '+') {
            username = username.substr(1); /* IGNORED */
        }
        else if (username[0] == '@' && username[1] == '@') {
            username = username.substr(2); /* IGNORED */
        }
        if (username[0] == '@') {
            username = username.substr(1);
            is_operator = true;
        }
        auto user = *(client_database_->GetUser(username));
        if (user) {
            AddUserToChannel(channel, user, is_operator);
        }
        else {
            logger_.Log(LogLevel::ERROR, "Could not add %s to channel, user does not exits.", username.c_str());
        }
    }
}

auto NJOINHandler::AddUserToChannel(IChannel* channel, IUser* user, bool is_operator) -> void
{
	if (channel->HasUser(user->GetUUID()))
	{
        logger_.Log(LogLevel::ERROR, "Could not add %s to channel, user is already part of the channel.", user->GetNickname().c_str());
		return ;
	}
	channel->AddUser(user, is_operator);

	std::string join_message = ":" + user->GetPrefix() + " JOIN :" + channel->GetName();
	channel->PushToLocal(join_message, std::nullopt);
}
