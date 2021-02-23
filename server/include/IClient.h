#ifndef __I_CLIENT_H__
#define __I_CLIENT_H__

#include <string>
#include <stdexcept>
#include <optional>
#include "UUID.h"

class IClient
{

    public:

    enum Type
    {
        kUnInitialized = 0,
        kLocalUser,
        kRemoteUser,
        kServer
    };

    enum State
    {
        kUnRegistered = 0,
        kRegistered,
        kDisconnected
    };

    IClient() : state_(IClient::State::kUnRegistered), type_(IClient::Type::kUnInitialized)
    {};

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
    virtual auto GetUUID() const -> const IRC::UUID& = 0;

    virtual auto GetState() const -> IClient::State { return state_; }
    virtual auto SetState(IClient::State state) -> void { state_ = state; }
    virtual auto GetPassword() const -> const std::string& { return password_; }
    virtual auto SetPassword(std::string password) -> void { password_ = password; }
    virtual auto GetNickname() const -> const std::string& { return nickname_; }
    virtual auto SetNickname(std::string nickname) -> void { nickname_ = nickname; }
    virtual auto GetType() const -> IClient::Type { return type_; }
    virtual auto SetType(IClient::Type type) -> void { type_ = type; }
    virtual auto GetUsername() const -> const std::string& { return username_; }
    virtual auto SetUsername(std::string username) -> void { username_ = username; }
    virtual auto GetRealname() const -> const std::string& { return realname_; }
    virtual auto SetRealname(std::string realname) -> void { realname_ = realname; }

    virtual ~IClient() {};

    class Disconnected : public std::runtime_error
    {
    public:
        Disconnected(const char *msg) : std::runtime_error(msg) {}
    };

    protected:

    State state_;
    std::string password_;
    std::string nickname_;
    std::string username_;
    std::string realname_;
    Type type_;
};

#endif
