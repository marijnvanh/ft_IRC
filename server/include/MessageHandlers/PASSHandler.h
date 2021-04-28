#ifndef PASS_HANDLER_HPP
#define PASS_HANDLER_HPP

#include "CommandHandler.h"

class PASSHandler : public CommandHandler
{
    public:
    PASSHandler(IClientDatabase *client_database);
    ~PASSHandler();

    auto SafeHandle(IMessage &message) -> void override;
};

#endif
