#ifndef __I_CLIENT_H__
#define __I_CLIENT_H__

#include <string>
#include <memory>
#include <stdexcept>
#include <optional>

class IClient
{
    public:

    //TODO Refine if we need this
    enum Type
    {
        kUnInitialized = 0,
        kClient,
        kServer
    };
    //TODO refine states
    enum State
    {
        kUnRegistered = 0,
        kDisconnecting,
        kDisconnected
    };
    virtual auto Push(std::shared_ptr<std::string> irc_message) -> void = 0;
    virtual auto Receive() -> std::optional<std::string> = 0;
    auto GetState() const -> IClient::State { return state_; }

    virtual ~IClient() {};

    protected:

    // Type type_;
    State state_;
    int UUID_;
};

#endif