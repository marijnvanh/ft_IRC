#ifndef KILL_HANDLER_HPP
#define KILL_HANDLER_HPP

#include "CommandHandler.h"

class KILLHandler : public CommandHandler
{
    public:
    
    KILLHandler(IClientDatabase *client_database);
    ~KILLHandler();
    
    auto SafeHandle(IMessage &message) -> void override;

	auto GetCorrectSender(IClient **client, IMessage &message) -> bool;
	
	auto HandleKillForLocalUser(IClient *client, IUser *otherUser,
		std::vector<std::string> &params) -> void;
	auto HandleKillForRemoteUser(IClient *client, IUser *otherUser,
		std::vector<std::string> &params) -> void;

};

#endif
