#ifndef SERVER_HANDLER_HPP
#define SERVER_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"
#include "IServerConfig.h"
#include "IServer.h"

class SERVERHandler : public ICommandHandler
{
    public:
    SERVERHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~SERVERHandler();

    auto Handle(IMessage &message) -> void override;

    private:

    auto HandleLocalServerRegistration(IClient *client, const std::string &server_name) -> void;
    auto HandleRemoteServerRegistration(IServer *local_server, IServer *remote_server, const std::string &server_name) -> void;

    IServerConfig *server_config_;
    IClientDatabase *client_database_;
    Logger logger;
};

#endif
