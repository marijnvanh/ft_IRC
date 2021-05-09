#ifndef NUMERIC_HANDLER_HPP
#define NUMERIC_HANDLER_HPP

#include "CommandHandler.h"

/**
 * @brief Numeric handler is only used to forward numeric messages
 * 
 */
class NUMERICHandler : public CommandHandler
{
    public:
    NUMERICHandler(IServerConfig* server_config_,
		IClientDatabase *client_database);
    ~NUMERICHandler();

    auto SafeHandle(IMessage &message) -> void override;
};

#endif
