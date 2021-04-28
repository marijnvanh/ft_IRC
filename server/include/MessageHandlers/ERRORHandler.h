#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"
#include "IServerConfig.h"

class ERRORHandler : public ICommandHandler
{
    public:
    ERRORHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~ERRORHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IServerConfig *server_config_;
    IClientDatabase *client_database_;
    Logger logger;
};

#endif
