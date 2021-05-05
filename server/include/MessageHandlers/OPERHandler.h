#ifndef OPER_HANDLER_HPP
#define OPER_HANDLER_HPP

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
};

#endif
