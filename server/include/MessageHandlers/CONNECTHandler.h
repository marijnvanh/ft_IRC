#ifndef CONNECT_HANDLER_HPP
#define CONNECT_HANDLER_HPP

#include "IRCServer.h"
#include "CommandHandler.h"

class CONNECTHandler : public CommandHandler
{
    public:
	
    CONNECTHandler(IServerConfig *server_config,
                    IClientDatabase *client_database,
                    IRCServer *irc_server);
    ~CONNECTHandler();

    auto SafeHandle(IMessage &message) -> void override;

    private:

    IRCServer *irc_server_;

};

#endif
