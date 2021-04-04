#ifndef QUIT_HANDLER_HPP
#define QUIT_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "Logger.h"

class QUITHandler : public ICommandHandler
{
    public:
    
    QUITHandler(IClientDatabase *client_database);
    ~QUITHandler();
    
    auto Handle(IMessage &message) -> void override;
    
    private:
    IClientDatabase *client_database_;
    Logger logger;

    auto GetQuitMessage(IMessage &message) -> std::string;
    auto DisconnectLocalUser(IMessage &message) -> void;
    auto DisconnectRemoteUser(IClient* remote_user, IMessage &message) -> void;

};

#endif