#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"

class ERRORHandler : public ICommandHandler
{
    public:
    ERRORHandler(IClientDatabase *client_database);
    ~ERRORHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IClientDatabase *client_database_;
    Logger logger;
};

#endif
