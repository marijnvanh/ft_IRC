#include "Message.h"

Message::Message(IRC::UUID uuid, IRC::RawMessage message) :
    uuid_(uuid),
    raw_message_(message)
{}

Message::~Message()
{}

auto Message::GetOriginType() const noexcept -> OriginType {
	if (raw_message_.prefix && raw_message_.prefix->is_server_origin)
		return (OriginType::SERVER);
	return (OriginType::USER);
}

auto Message::GetCommand() const noexcept -> const std::string &{
    return raw_message_.command.name;
}

auto Message::GetParams() const noexcept -> const std::vector<std::string> & {
    return raw_message_.command.parameters;
}

auto Message::GetClientUUID() const noexcept -> const IRC::UUID {
    return uuid_;
}

auto Message::GetPrefix() const noexcept -> const std::optional<std::string>
{
	if (raw_message_.prefix)
		return raw_message_.prefix->name;
	return std::nullopt;
}

auto Message::GetNickname() const noexcept -> const std::optional<std::string>
{
	if (raw_message_.prefix && !raw_message_.prefix->is_server_origin)
		return raw_message_.prefix->name;
	return std::nullopt;
}

auto Message::GetServername() const noexcept -> const std::optional<std::string>
{
    if (raw_message_.prefix && raw_message_.prefix->is_server_origin)
        return raw_message_.prefix->name;
    return std::nullopt;
}
