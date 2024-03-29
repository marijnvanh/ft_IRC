#ifndef QUIT_HANDLER_HPP
#define QUIT_HANDLER_HPP

#include "CommandHandler.h"

class QUITHandler : public CommandHandler
{
    public:
    
    QUITHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~QUITHandler();
    
    auto SafeHandle(IMessage &message) -> void override;
    
    private:

    auto GetQuitMessage(IMessage &message) -> std::string;
    auto DisconnectLocalUser(IMessage &message) -> void;
    auto DisconnectRemoteUser(IClient* remote_user, IMessage &message) -> void;

};

#endif
