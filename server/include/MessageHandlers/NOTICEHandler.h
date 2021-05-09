#ifndef NOTICE_HANDLER_HPP
#define NOTICE_HANDLER_HPP

#include "CommandHandler.h"
#include "IChannelDatabase.h"

class NOTICEHandler : public CommandHandler
{
    public:
    NOTICEHandler(IServerConfig *server_config, IClientDatabase *client_database);
    ~NOTICEHandler();

    auto SafeHandle(IMessage &message) -> void override;

};

#endif
