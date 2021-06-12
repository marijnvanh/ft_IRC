#ifndef PING_HANDLER_HPP
#define PING_HANDLER_HPP

#include "CommandHandler.h"

class PINGHandler : public CommandHandler
{

public:
    PINGHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~PINGHandler();

    auto SafeHandle(IMessage &message) -> void override;

private:
	/**
	*	@brief Handles PING when a target parameter is specified.
	*
	*	@returns Whether or not the PING has been forwarded.
	*	Will therefore return `false` if the target is the current server.
	*/
	auto HandleForTarget(IClient* client, IMessage &message) -> bool;

	auto HandleForThisServer(IClient* client, IMessage &message) -> void;
};

#endif
