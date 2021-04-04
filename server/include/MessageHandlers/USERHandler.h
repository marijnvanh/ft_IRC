#ifndef USER_HANDLER_HPP
#define USER_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "IServerConfig.h"
#include "Logger.h"

class USERHandler : public ICommandHandler
{
    public:
    
    USERHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~USERHandler();
    
    auto Handle(IMessage &message) -> void override;
    
    private:
    IServerConfig *server_config_;
    IClientDatabase *client_database_;
    Logger logger;

    auto HandleUSERFromUser(IMessage &message) -> void;

};

#endif