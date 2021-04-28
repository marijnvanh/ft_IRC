#ifndef JOIN_HANDLER_HPP
#define JOIN_HANDLER_HPP

#include "CommandHandler.h"
#include "IChannelDatabase.h"

class JOINHandler : public CommandHandler
{
    public:
    JOINHandler(IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~JOINHandler();

    auto SafeHandle(IMessage &message) -> void override;

    private:
	
    IChannelDatabase *channel_database_;
};

#endif
