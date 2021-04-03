#ifndef NICK_HANDLER_HPP
#define NICK_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "IServerConfig.h"
#include "Logger.h"

class NICKHandler : public ICommandHandler
{
    public:
    
    NICKHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~NICKHandler();
    
    auto Handle(IMessage &message) -> void override;
    
    private:
    IServerConfig *server_config_;
    IClientDatabase *client_database_;
    Logger logger;

    auto HandleNewRemoteUser(IClient* server, IMessage &message) -> void;
    auto HandleNICKFromUser(IClient* client, IMessage &message) -> void;
    auto HandleNicknameChangeFromServer(IClient* server, IMessage &message) -> void;

};

#endif