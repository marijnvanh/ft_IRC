#ifndef IMESSAGE_DISPATCHER_HPP
#define IMESSAGE_DISPATCHER_HPP

#include <string>

#include "Message.h"

class IMessageDispatcher
{
    public:
    virtual ~IMessageDispatcher() {};
    virtual auto Dispatch(Message message) -> bool = 0;
};

#endif