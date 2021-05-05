#ifndef USER_HANDLER_HPP
#define USER_HANDLER_HPP

#include "CommandHandler.h"

class USERHandler : public CommandHandler
{
    public:
    
    USERHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~USERHandler();
    
    auto SafeHandle(IMessage &message) -> void override;
    
    private:

    auto HandleUSERFromUser(IMessage &message) -> void;

};

#endif
