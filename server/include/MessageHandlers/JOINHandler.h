#ifndef JOIN_HANDLER_HPP
#define JOIN_HANDLER_HPP

#include "Logger.h"
#include "IMessage.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"

class JOINHandler : public ICommandHandler
{
    public:
    JOINHandler(IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~JOINHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IClientDatabase *client_database_;
    IChannelDatabase *channel_database_;
    Logger logger;
};

#endif
