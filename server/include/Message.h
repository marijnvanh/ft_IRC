#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include "IMessage.h"
#include "RawMessage.h"

class Message : public IMessage
{
public:
    Message(IRC::UUID uuid, IRC::RawMessage message);
    ~Message();

	auto GetOriginType() const noexcept -> OriginType override;

    auto GetCommand() const noexcept -> const std::string & override;
    auto GetParams() const noexcept -> const std::vector<std::string> & override;
    
	auto GetClientUUID() const noexcept -> const IRC::UUID override;
	auto GetPrefix() const noexcept -> const std::optional<std::string> override;
    auto GetNickname() const noexcept -> const std::optional<std::string> override;
    auto GetServername() const noexcept -> const std::optional<std::string> override;

private:
    IRC::UUID uuid_;
    IRC::RawMessage raw_message_;
};

#endif
