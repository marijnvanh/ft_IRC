#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include <string>
#include "RawMessage.h"
#include "UUID.h"

class Message
{
public:
    Message(IRC::UUID uuid, IRC::RawMessage message);
    ~Message();

    auto GetUUID() const noexcept -> const IRC::UUID;
    auto GetCommand() const noexcept -> const std::string;
    auto GetParams() const noexcept -> const std::vector<std::string>;

private:
    IRC::UUID uuid_;
    IRC::RawMessage raw_message_;
};

inline std::ostream &operator<<(std::ostream &os, const Message &message)
{
    os << "Message(uuid: " << message.GetUUID() << ", "
       << "command: \"" << message.GetCommand() << "\")";
    return os;
}

#endif
