#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include "IServer.h"
#include "CommandHandler.h"

class SERVERHandler : public CommandHandler
{
    public:
	
    SERVERHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~SERVERHandler();

    auto SafeHandle(IMessage &message) -> void override;

    private:

    auto HandleBroadcasting(IClient *new_server, IMessage &message) -> void;
    auto HandleLocalServerRegistration(IClient *client, IMessage &message) -> void;
    auto HandleRemoteServerRegistration(IServer *local_server, IServer *remote_server, IMessage &message) -> void;
};

#endif
