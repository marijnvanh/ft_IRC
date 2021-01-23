#ifndef __I_CLIENT_H__
#define __I_CLIENT_H__

#include <string>
#include <queue>
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

    /**
     * @brief Push a message on to the send queue (later to be send by SendAll)
     * 
     * @param irc_message 
     */
    virtual auto Push(std::shared_ptr<std::string> irc_message) -> void = 0;
    
    /**
     * @brief Receive message from Client, will return nullopt when nothing to receive
     * 
     * @exception IClient::Closed
     * 
     * @return std::optional<std::string> containing either a message or nullopt
     */
    virtual auto Receive() -> std::optional<std::string> = 0;
    
    /**
     * @brief Send all messages in outgoing message queue
     * 
     */
    virtual auto SendAll() -> void = 0;

    auto GetState() const -> IClient::State { return state_; }
    auto virtual GetUUID() const -> int { return UUID_; }

    virtual ~IClient() {};

    class Disconnected : public std::runtime_error
    {
    public:
        Disconnected(const char *msg) : std::runtime_error(msg) {}
    };

    protected:

    // Type type_;
    std::queue<std::shared_ptr<std::string>> outgoing_msg_queue_;
    State state_;
    int UUID_;
};

#endif