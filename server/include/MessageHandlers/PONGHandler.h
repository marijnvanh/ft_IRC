#ifndef PONG_HANDLER_HPP
#define PONG_HANDLER_HPP

#include "CommandHandler.h"

class PONGHandler : public CommandHandler
{

public:
    PONGHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~PONGHandler();

    auto SafeHandle(IMessage &message) -> void override;

private:	
	auto HandleForThisServer(IClient *client) -> void;
	auto HandleForTarget(IClient *client, std::string &target_name,
		IMessage &message) -> void;
};

#endif
