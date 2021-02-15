#include "Message.h"

const auto Message::GetUUID() const noexcept -> IRC::UUID {
    return uuid_;
}

const auto Message::GetCommand() const noexcept -> std::string {
    return message_.command.name;
}

const auto Message::GetParams() const noexcept -> std::vector<std::string> {
    return message_.command.parameters;
}
