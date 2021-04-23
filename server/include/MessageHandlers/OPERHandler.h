#ifndef OPER_HANDLER_HPP
#define OPER_HANDLER_HPP

#include "Logger.h"
#include "IServerConfig.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"

/**
 * @brief Oper handler is used to register server operators. 
 */
class OPERHandler : public ICommandHandler
{
	public:
    OPERHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~OPERHandler();

    auto Handle(IMessage &message) -> void override;

    private:
	IServerConfig *server_config_;
    IClientDatabase *client_database_;
    
	Logger logger;
};

#endif
