#ifndef __I_CLIENT_H__
#define __I_CLIENT_H__

#include <string>
#include <stdexcept>
#include <optional>
#include "UUID.h"

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

    enum State
    {
        kUnRegistered = 0,
        kRegistered,
        kDisconnecting,
        kDisconnected
    };

    /**
     * @brief Push a message on to the send queue (later to be send by SendAll)
     * 
     * @param irc_message 
     */
    virtual auto Push(std::string irc_message) -> void = 0;
    
    /**
     * @brief Receive message from Client, will return nullopt when nothing to receive
     * 
     * @exception IClient::Disconnected
     * 
     * @return std::optional<std::string> containing either a message or nullopt
     */
    virtual auto Receive() -> std::optional<std::string> = 0;
    
    /**
     * @brief Send all messages in outgoing message queue
     * 
     * @exception IClient::Disconnected
     */
    virtual auto SendAll() -> void = 0;
    virtual auto GetUUID() const -> IRC::UUID = 0;

    virtual auto GetState() const -> IClient::State { return state_; }
    virtual auto SetState(IClient::State state) -> void { state_ = state; }
    virtual auto GetPassword() const -> std::string { return password_; }
    virtual auto SetPassword(std::string password) -> void { password_ = password; }

    virtual ~IClient() {};

    class Disconnected : public std::runtime_error
    {
    public:
        Disconnected(const char *msg) : std::runtime_error(msg) {}
    };

    protected:

    // Type type_;
    State state_;
    std::string password_;
};

#endif
