#ifndef PART_HANDLER_HPP
#define PART_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"
#include "IServerConfig.h"

class PARTHandler : public ICommandHandler
{
    public:
    PARTHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~PARTHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IServerConfig *server_config_;
    IClientDatabase *client_database_;
    IChannelDatabase *channel_database_;
    Logger logger;

    auto StartPartParsing(std::vector<std::string> params, IClient* client) -> void;
};

#endif
