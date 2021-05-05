#ifndef COMMAND_HANDLER_HPP
#define COMMAND_HANDLER_HPP

#include "Logger.h"
#include "IServerConfig.h"
#include "IClientDatabase.h"
#include "ICommandHandler.h"

#include <stdint.h>

class CommandHandler : public ICommandHandler
{

public:

	CommandHandler(IServerConfig* server_config,
		IClientDatabase *client_database,
		std::string command_name,
		uint8_t min_params = 0,
		bool allow_unregistered = false);

    auto Handle(IMessage &message) -> void override;

	virtual auto SafeHandle(IMessage &message) -> void = 0;

private:

	uint8_t min_params_;
	bool allow_unregistered_;

protected:

	Logger logger_;
	IServerConfig* server_config_;
	IClientDatabase* client_database_;

	std::string command_name_;

};

#endif
