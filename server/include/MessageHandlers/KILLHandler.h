#ifndef KILL_HANDLER_HPP
#define KILL_HANDLER_HPP

#include "Logger.h"
#include "IClientDatabase.h"
#include "MessageHandlers/ICommandHandler.h"

class KILLHandler : public ICommandHandler
{
    public:
    
    KILLHandler(IClientDatabase *client_database);
    ~KILLHandler();
    
    auto Handle(IMessage &message) -> void override;

	auto GetCorrectSender(IClient **client, IMessage &message) -> bool;
	
	auto HandleKillForLocalUser(IClient &client, IUser &otherUser,
		std::vector<std::string> params) -> void;
	auto HandleKillForRemoteUser(IClient &client, IUser &otherUser,
		std::vector<std::string> params) -> void;
    
    private:
    IClientDatabase *client_database_;
    Logger logger;

};

#endif
