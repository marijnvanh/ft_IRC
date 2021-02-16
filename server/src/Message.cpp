#include "Message.h"

Message::Message(IRC::UUID uuid, IRC::RawMessage message) 
    : uuid_(uuid), raw_message_(message) 
{}

Message::~Message()
{}

auto Message::GetUUID() const noexcept -> const IRC::UUID {
    return uuid_;
}

auto Message::GetCommand() const noexcept -> const std::string {
    return raw_message_.command.name;
}

auto Message::GetParams() const noexcept -> const std::vector<std::string> {
    return raw_message_.command.parameters;
}
