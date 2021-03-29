#ifndef PART_HANDLER_HPP
#define PART_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"

class PARTHandler : public ICommandHandler
{
    public:
    PARTHandler(IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~PARTHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IClientDatabase *client_database_;
    IChannelDatabase *channel_database_;
    Logger logger;
};

#endif
