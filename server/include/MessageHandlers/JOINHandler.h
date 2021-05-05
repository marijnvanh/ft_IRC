#ifndef JOIN_HANDLER_HPP
#define JOIN_HANDLER_HPP

#include "CommandHandler.h"
#include "IChannelDatabase.h"

class JOINHandler : public CommandHandler
{
    public:
    JOINHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~JOINHandler();

    auto SafeHandle(IMessage &message) -> void override;


    private:
	
    IChannelDatabase *channel_database_;

    auto CreateChannelKeyMap(std::string param_name, std::string param_key) -> const std::map<std::string, std::string>;
    auto TryAddUserToChannel(IChannel* channel, const std::string key, IUser* user, bool isOp) -> bool;
    auto StartJoinParsing(const std::vector<std::string> &params, IClient* client, IChannelDatabase *channel_database) -> void;
};

#endif
