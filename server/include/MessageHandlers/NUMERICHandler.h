#ifndef NUMERIC_HANDLER_HPP
#define NUMERIC_HANDLER_HPP

#include "Logger.h"
#include "ICommandHandler.h"
#include "IClientDatabase.h"

/**
 * @brief Numeric handler is only used to forward numeric messages
 * 
 */
class NUMERICHandler : public ICommandHandler
{
    public:
    NUMERICHandler(IClientDatabase *client_database);
    ~NUMERICHandler();

    auto Handle(IMessage &message) -> void override;

    private:
    IClientDatabase *client_database_;
    Logger logger;
};

#endif
