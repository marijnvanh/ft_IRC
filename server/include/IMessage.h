#ifndef __IMESSAGE_H__
#define __IMESSAGE_H__

#include <string>
#include <memory>
#include <optional>

#include "UUID.h"

class IMessage
{
public:
    virtual ~IMessage() {};

    virtual auto GetClientUUID() const noexcept -> const IRC::UUID = 0;
    virtual auto GetCommand() const noexcept -> const std::string & = 0;
    virtual auto GetParams() const noexcept -> const std::vector<std::string> & = 0;
    virtual auto GetServername() const noexcept -> const std::optional<std::string> = 0;
    virtual auto GetNickname() const noexcept -> const std::optional<std::string> = 0;
};

inline std::ostream &operator<<(std::ostream &os, const IMessage &message)
{
    os << "Message(" << message.GetClientUUID() << ", "
       << "command: \"" << message.GetCommand() << "\")";
    return os;
}

#endif
