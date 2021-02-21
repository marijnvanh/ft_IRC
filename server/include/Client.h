#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>
#include <stdexcept>
#include <optional>
#include <queue>

#include "IClient.h"
#include "IIOHandler.h"


class Client : public IClient
{
    public:

    Client(std::unique_ptr<IRC::IIOHandler> io_handler);
    ~Client();

    auto Push(std::string irc_message) -> void override;
    auto Receive() -> std::optional<std::string> override;
    auto SendAll() -> void override;

    auto GetUUID() const -> const IRC::UUID& override { return uuid_; };

    private:
    std::queue<std::string> outgoing_msg_queue_;
    std::unique_ptr<IRC::IIOHandler> io_handler_;
    IRC::UUID uuid_;
};

#endif
