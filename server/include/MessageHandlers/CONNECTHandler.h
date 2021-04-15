#ifndef CONNECT_HANDLER_HPP
#define CONNECT_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"
#include "IServerConfig.h"
#include "IRCServer.h"

class CONNECTHandler : public ICommandHandler
{
    public:
    CONNECTHandler(IServerConfig *server_config,
                    IClientDatabase *client_database,
                    IRCServer *irc_server);
    ~CONNECTHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IClientDatabase *client_database_;
    IServerConfig *server_config_;
    IRCServer *irc_server_;
    Logger logger;

};

#endif
