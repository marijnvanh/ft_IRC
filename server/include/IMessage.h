#ifndef __IMESSAGE_H__
#define __IMESSAGE_H__

#include <string>
#include <memory>
#include <optional>

#include "IClient.h"
#include "Mutex.h"

class IMessage
{
public:
    virtual ~IMessage() {};

    virtual auto GetUUID() const noexcept -> const IRC::UUID = 0;
    virtual auto GetCommand() const noexcept -> const std::string & = 0;
    virtual auto GetParams() const noexcept -> const std::vector<std::string> & = 0;
    virtual auto GetServername() const noexcept -> const std::optional<std::string> = 0;
    virtual auto GetNickname() const noexcept -> const std::optional<std::string> = 0;
    virtual auto GetClient() const noexcept -> std::shared_ptr<IRC::Mutex<IClient>> = 0;
};

inline std::ostream &operator<<(std::ostream &os, const IMessage &message)
{
    os << "Message(" << message.GetUUID() << ", "
       << "command: \"" << message.GetCommand() << "\")";
    return os;
}

#endif
