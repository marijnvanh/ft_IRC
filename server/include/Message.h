#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include <string>
#include "RawMessage.h"
#include "UUID.h"

class Message
{
public:
    Message(IRC::UUID uuid, IRC::RawMessage message) 
        : uuid_(uuid),
          message_(message) 
    {}

    const auto GetUUID() const noexcept -> IRC::UUID;
    const auto GetCommand() const noexcept -> std::string;
    const auto GetParams() const noexcept -> std::vector<std::string>;
private:
    IRC::UUID uuid_;
    IRC::RawMessage message_;
};

inline std::ostream& operator<<(std::ostream& os, const Message& message) {
    os << "Message(uuid: " << message.GetUUID() << ", " <<
                  "command: \"" << message.GetCommand() << "\")";
    return os;
}

#endif
