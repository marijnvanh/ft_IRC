#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "CommandHandler.h"

class ERRORHandler : public CommandHandler
{
    public:
    ERRORHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~ERRORHandler();

    auto SafeHandle(IMessage &message) -> void override;
};

#endif
