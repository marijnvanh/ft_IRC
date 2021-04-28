#ifndef NICK_HANDLER_HPP
#define NICK_HANDLER_HPP

#include "Logger.h"
#include "IServerConfig.h"
#include "CommandHandler.h"

class NICKHandler : public CommandHandler
{
    public:
    
    NICKHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~NICKHandler();
    
    auto SafeHandle(IMessage &message) -> void override;
    
    private:

    IServerConfig *server_config_;

    auto HandleNewRemoteUser(IClient* server, IMessage &message) -> void;
    auto HandleNICKFromUser(IClient* client, IMessage &message) -> void;
    auto HandleNicknameChangeFromServer(IClient* server, IMessage &message) -> void;

};

#endif
