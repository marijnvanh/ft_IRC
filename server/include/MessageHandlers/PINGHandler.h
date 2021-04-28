#ifndef PING_HANDLER_HPP
#define PING_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "IServerConfig.h"

class PINGHandler : public ICommandHandler
{
    public:
    PINGHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~PINGHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IServerConfig *server_config_;
};

#endif