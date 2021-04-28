#ifndef JOIN_HANDLER_HPP
#define JOIN_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"
#include "IServerConfig.h"

class JOINHandler : public ICommandHandler
{
    public:
    JOINHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~JOINHandler();

    auto Handle(IMessage &message) -> void override;


    private:
    IServerConfig *server_config_;
    IClientDatabase *client_database_;
    IChannelDatabase *channel_database_;
    Logger logger;

    auto CreateChannelKeyMap(std::string param_name, std::string param_key) -> const std::map<std::string, std::string>;
    auto TryAddUserToChannel(IChannel* channel, const std::string key, IUser* user, bool isOp) -> bool;
    auto StartJoinParsing(const std::vector<std::string> &params, IClient* client, IChannelDatabase *channel_database) -> void;
};

#endif
