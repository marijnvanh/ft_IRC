#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include <string>
#include "IMessage.h"
#include "RawMessage.h"
#include "UUID.h"

class Message : public IMessage
{
public:
    Message(IRC::UUID uuid, IRC::RawMessage message);
    ~Message();

    auto GetUUID() const noexcept -> const IRC::UUID override;
    auto GetCommand() const noexcept -> const std::string & override;
    auto GetParams() const noexcept -> const std::vector<std::string> & override;
    auto GetServername() const noexcept -> const std::optional<std::string> override;
    auto GetNickname() const noexcept -> const std::optional<std::string> override;

private:
    IRC::UUID uuid_;
    IRC::RawMessage raw_message_;
};

#endif
