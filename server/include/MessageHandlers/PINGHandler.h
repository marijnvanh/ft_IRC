#ifndef PING_HANDLER_HPP
#define PING_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"

class PINGHandler : public ICommandHandler
{
    public:
    PINGHandler(IClientDatabase *client_database);
    ~PINGHandler();

    auto Handle(IMessage &message) -> void override;
};

#endif