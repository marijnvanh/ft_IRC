#ifndef KILL_HANDLER_HPP
#define KILL_HANDLER_HPP

#include "Logger.h"
#include "IClientDatabase.h"
#include "MessageHandlers/ICommandHandler.h"
#include "IServerConfig.h"

class KILLHandler : public ICommandHandler
{
    public:
    
    KILLHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~KILLHandler();
    
    auto Handle(IMessage &message) -> void override;

	auto GetCorrectSender(IClient **client, IMessage &message) -> bool;
	
	auto HandleKillForLocalUser(IClient *client, IUser *otherUser,
		std::vector<std::string> &params) -> void;
	auto HandleKillForRemoteUser(IClient *client, IUser *otherUser,
		std::vector<std::string> &params) -> void;
    
    private:
    IServerConfig *server_config_;
    IClientDatabase *client_database_;
    Logger logger;

};

#endif
