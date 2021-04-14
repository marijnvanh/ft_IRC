#ifndef SQUIT_HANDLER_HPP
#define SQUIT_HANDLER_HPP

#include "Logger.h"
#include "IServerConfig.h"
#include "IClientDatabase.h"
#include "MessageHandlers/ICommandHandler.h"

class SQUITHandler : public ICommandHandler
{

public:

    SQUITHandler(IServerConfig * server_config,
		IClientDatabase *client_database);
    ~SQUITHandler();

    auto Handle(IMessage &message) -> void override;
	
	auto GetOriginalSender(IClient **client, IMessage &message) -> bool;

	auto HandleUserMessage(IUser *user,
		std::vector<std::string> params) -> void;
	auto HandleServerMessage(IServer *server,
		std::vector<std::string> params) -> void;

 private:

	IServerConfig *server_config_;
    IClientDatabase *client_database_;
    Logger logger;

};

#endif