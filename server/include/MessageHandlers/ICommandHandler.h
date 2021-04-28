#ifndef ICOMMAND_HANDLER_HPP
#define ICOMMAND_HANDLER_HPP

#include "IMessage.h"

class ICommandHandler
{
    public:
    virtual ~ICommandHandler(){};

    virtual auto Handle(IMessage &message) -> void = 0;

	// Make method here bitch, don't put members plz.
};

#endif
