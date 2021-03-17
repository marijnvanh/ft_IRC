#ifndef PASS_HANDLER_HPP
#define PASS_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "Logger.h"

class PASSHandler : public ICommandHandler
{
    public:
    PASSHandler(IClientDatabase *client_database);
    ~PASSHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IClientDatabase *client_database_;
    Logger logger;
};

#endif