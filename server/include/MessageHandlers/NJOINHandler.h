#ifndef NJOIN_HANDLER_HPP
#define NJOIN_HANDLER_HPP


#include "CommandHandler.h"
#include "IChannelDatabase.h"

class NJOINHandler : public CommandHandler
{
	public:

    NJOINHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~NJOINHandler();

    auto SafeHandle(IMessage &message) -> void override;

 	private:

    auto AddUsersToChannel(IChannel* channel, std::vector<std::string> &usernames) -> void;
    auto AddUserToChannel(IChannel* channel, IUser* user, bool is_operator) -> void;

    IChannelDatabase *channel_database_;

};

#endif
