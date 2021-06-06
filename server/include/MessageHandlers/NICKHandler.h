#ifndef NICK_HANDLER_HPP
#define NICK_HANDLER_HPP

#include "CommandHandler.h"

class NICKHandler : public CommandHandler
{
    public:
    
    NICKHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~NICKHandler();
    
    auto SafeHandle(IMessage &message) -> void override;
    
    private:

    auto HandleNewRemoteUser(IClient* server, IMessage &message) -> void;
    auto HandleNICKFromUser(IClient* client, IMessage &message) -> void;
    auto HandleNicknameChangeFromServer(IClient* server, IMessage &message) -> void;
    auto HandleNickCollision(IClient* server, IClient* client_with_same_nickname) -> void;

};

#endif
