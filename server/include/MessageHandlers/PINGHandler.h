#ifndef PING_HANDLER_HPP
#define PING_HANDLER_HPP

#include "CommandHandler.h"

class PINGHandler : public CommandHandler
{
    public:
    PINGHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~PINGHandler();

    auto SafeHandle(IMessage &message) -> void override;
};

#endif
