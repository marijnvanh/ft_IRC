#ifndef PONG_HANDLER_HPP
#define PONG_HANDLER_HPP

#include "CommandHandler.h"

class PONGHandler : public CommandHandler
{
    public:
    PONGHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~PONGHandler();

    auto SafeHandle(IMessage &message) -> void override;
};

#endif
