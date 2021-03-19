#ifndef __NAMES_HANDLER_H__
#define __NAMES_HANDLER_H__

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"
#include "Logger.h"

class NAMESHandler : public ICommandHandler
{
    public:
    NAMESHandler(IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~NAMESHandler();

    auto Handle(IMessage &message) -> void override;
    auto HandleNAMREPLY(IClient *receiver, std::vector<std::string> &channel_list) -> void;
    auto HandleNAMREPLY(IClient *receiver) -> void;

    private:
    IClientDatabase *client_database_;
    IChannelDatabase *channel_database_;
    Logger logger;

};

#endif