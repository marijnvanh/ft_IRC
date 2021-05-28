#include "Numerics.h"
#include "Utilities.h"
#include "MessageHandlers/NAMESHandler.h"

#define CHANNEL_LIST 0
/*
      Command: NAMES
   Parameters: [<channel>{,<channel>}]

   By using the NAMES command, a user can list all nicknames in a channel.

   If no <channel> parameter is given, a list of all channels and their
   occupants is returned.  At the end of this list, a list of users who
   are visible but not on any channel are listed as being on `channel' "*".

   Numerics:

           RPL_NAMREPLY                    RPL_ENDOFNAMES

   Examples:

   NAMES #twilight_zone,#42        ; list visible users on #twilight_zone
                                   and #42 if the channels are visible to
                                   you.

   NAMES                           ; list all visible channels and users
*/

NAMESHandler::NAMESHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database) :
	CommandHandler(server_config, client_database, "NAMES"),
    channel_database_(channel_database)
{}

NAMESHandler::~NAMESHandler()
{}

auto NAMESHandler::SafeHandle(IMessage &message) -> void
{
    auto client = *(client_database_->GetClient(message.GetClientUUID()));
    
    /* Names message can't come from a server, so we just ignore it if it does */
    if (client->GetType() == IClient::Type::kLocalServer)
    {
        return ;
    }

    auto params = message.GetParams();
    if (params.size() == 0)
        HandleNAMREPLY(client);
    else
    {
        auto channel_list = split(params[CHANNEL_LIST], ",");
        HandleNAMREPLY(client, channel_list);
    }
}

static auto SendNAMREPLY(IClient *receiver, IChannel *channel) -> void
{
    receiver->Push(std::to_string(RPL_NAMREPLY) + " = " + channel->GetName() + " :" + channel->GetUserListAsString(' '));
    receiver->Push(std::to_string(RPL_ENDOFNAMES) + " " + channel->GetName() + " :End of /NAMES list");
}

auto NAMESHandler::GetUsersNotInAChannel(std::optional<IRC::UUID> skip_uuid) -> std::string
{
    logger_.Log(LogLevel::DEBUG, "Get users not in a channel");
    std::string user_list;
    client_database_->DoForEachUser([&user_list](IClient* client)
        {
            auto user = dynamic_cast<IUser*>(client);
            auto channels = user->GetChannels();
            if (channels.empty() == true) {
                user_list = user_list + user->GetNickname() + " ";
            }

        }, skip_uuid);

    /* Remove extra space at end */
    if (user_list.empty() == false)
        user_list.pop_back();
    return user_list;
}

auto NAMESHandler::HandleNAMREPLY(IClient *receiver, std::vector<std::string> &channel_list) -> void
{
    for (std::string channel_name : channel_list)
    {
        auto channel = channel_database_->GetChannel(channel_name);

        if (channel == std::nullopt)
            continue ;
        
        SendNAMREPLY(receiver, *channel);
    }
}

auto NAMESHandler::HandleNAMREPLY(IClient *receiver) -> void
{
    channel_database_->ForEachChannel(
        [receiver](IChannel* channel)
        {
            SendNAMREPLY(receiver, channel);
        });
    auto user_list = GetUsersNotInAChannel(std::make_optional<IRC::UUID>(receiver->GetUUID()));
    if (user_list.empty() == false) {
        receiver->Push(std::to_string(RPL_NAMREPLY) + " * :" + user_list);
        receiver->Push(std::to_string(RPL_ENDOFNAMES) + " * :End of /NAMES list");
    }
}
