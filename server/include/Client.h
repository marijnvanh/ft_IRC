#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>
#include <stdexcept>
#include <optional>

#include "IClient.h"
#include "IIOHandler.h"


class Client : public IClient
{
    public:

    Client(std::unique_ptr<IRC::IIOHandler> io_handler);
    ~Client();

    auto Push(std::shared_ptr<std::string> irc_message) -> void override;
    auto Receive() -> std::optional<std::string> override;
    auto SendAll() -> void override;

    auto GetUUID() const -> IRC::UUID override { return UUID_; };

    private:
    std::unique_ptr<IRC::IIOHandler> io_handler_;
    IRC::UUID UUID_;
};

#endif
