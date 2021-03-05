#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <string>
#include <stdexcept>
#include <optional>
#include <queue>

#include "IClient.h"
#include "IIOHandler.h"
#include "IServer.h"


class Client : public virtual IClient
{
    public:

    Client(std::unique_ptr<IRC::IIOHandler> io_handler, IServer* server);
    ~Client();

    auto Push(std::string irc_message) -> void override;
    auto Receive() -> std::optional<std::string> override;
    auto SendAll() -> void override;

    auto GetUUID() const -> const IRC::UUID& override { return uuid_; };
    auto GetServer() -> IServer* override { return server_; };

    Client (Client&& other) : IClient(std::move(other)), uuid_(other.uuid_)
    {
        if (other.state_ != kUnRegistered)
            throw AlreadyRegistered("Can't move a client after registration");

        io_handler_ = std::move(other.io_handler_);
        outgoing_msg_queue_ = std::move(other.outgoing_msg_queue_);
        server_ = other.server_;
        uuid_ = other.uuid_;
    };

    Client (Client& other) = delete;
    Client &operator =(Client& other) = delete;
    Client &operator= (Client&& other) = delete;

    class AlreadyRegistered : public std::runtime_error
    {
    public:
        AlreadyRegistered(const char *msg) : std::runtime_error(msg) {}
    };

    private:
    std::queue<std::string> outgoing_msg_queue_;
    std::unique_ptr<IRC::IIOHandler> io_handler_;
    IServer* server_;
    IRC::UUID uuid_;
};

#endif
