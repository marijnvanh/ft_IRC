#ifndef OPER_HANDLER_HPP
#define OPER_HANDLER_HPP

#include "IServerConfig.h"
#include "CommandHandler.h"

/**
 * @brief Oper handler is used to register server operators. 
 */
class OPERHandler : public CommandHandler
{
	public:
    OPERHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~OPERHandler();

    auto SafeHandle(IMessage &message) -> void override;

    private:

	IServerConfig *server_config_;
};

#endif
