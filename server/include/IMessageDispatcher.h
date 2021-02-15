#ifndef IMESSAGE_DISPATCHER_HPP
#define IMESSAGE_DISPATCHER_HPP

#include <string>

class IMessageDispatcher
{
    public:
    virtual ~IMessageDispatcher() {};
    virtual auto Dispatch(std::string message) -> void = 0;
};

#endif