#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include "IMessage.h"
#include "RawMessage.h"

class Message : public IMessage
{
public:
    Message(std::shared_ptr<IClient> client, IRC::RawMessage message);
    ~Message();

    auto GetUUID() const noexcept -> const IRC::UUID override;
    auto GetCommand() const noexcept -> const std::string & override;
    auto GetParams() const noexcept -> const std::vector<std::string> & override;
    auto GetServername() const noexcept -> const std::optional<std::string> override;
    auto GetNickname() const noexcept -> const std::optional<std::string> override;
    auto GetClient() const noexcept -> std::shared_ptr<IClient> override;

private:
    std::shared_ptr<IClient> client_;
    IRC::UUID uuid_;
    IRC::RawMessage raw_message_;
};

#endif
