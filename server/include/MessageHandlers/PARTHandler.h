#ifndef PART_HANDLER_HPP
#define PART_HANDLER_HPP

#include "CommandHandler.h"
#include "IChannelDatabase.h"

class PARTHandler : public CommandHandler
{
    public:
    PARTHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~PARTHandler();

    auto SafeHandle(IMessage &message) -> void override;

    private:
    IChannelDatabase *channel_database_;

	auto GetOriginalSender(IClient **client, IMessage &message) -> bool;
    auto StartPartParsing(std::vector<std::string> params, IClient* client) -> void;
};

#endif
