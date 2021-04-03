#ifndef KILL_HANDLER_HPP
#define KILL_HANDLER_HPP

#include "MessageHandlers/ICommandHandler.h"
#include "IClientDatabase.h"
#include "Logger.h"

class KILLHandler : public ICommandHandler
{
    public:
    
    KILLHandler(IClientDatabase *client_database);
    ~KILLHandler();
    
    auto Handle(IMessage &message) -> void override;
    
    private:
    IClientDatabase *client_database_;
    Logger logger;

};

#endif
