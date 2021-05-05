#ifndef NAMES_HANDLER_H
#define NAMES_HANDLER_H

#include "CommandHandler.h"
#include "IChannelDatabase.h"

class NAMESHandler : public CommandHandler
{
    public:
    NAMESHandler(IServerConfig *server_config, IClientDatabase *client_database, IChannelDatabase *channel_database);
    ~NAMESHandler();

    auto SafeHandle(IMessage &message) -> void override;
   
    auto HandleNAMREPLY(IClient *receiver, std::vector<std::string> &channel_list) -> void;
    auto HandleNAMREPLY(IClient *receiver) -> void;

    private:

    IChannelDatabase *channel_database_;

};

#endif
