#ifndef VERSION_HANDLER_HPP
#define VERSION_HANDLER_HPP

#include "CommandHandler.h"

class VERSIONHandler : public CommandHandler
{
    public:
    VERSIONHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~VERSIONHandler();

    auto SafeHandle(IMessage &message) -> void override;
	
	auto GetOriginalSender(IClient **client, IMessage &message) -> bool;
};

#endif
