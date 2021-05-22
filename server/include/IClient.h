#ifndef __I_CLIENT_H__
#define __I_CLIENT_H__

#include <string>
#include <stdexcept>
#include <optional>
#include "UUID.h"

class IServer;

class IClient
{
    public:

    enum class Type
    {
        kUnRegistered = 0,
        kLocalUser,
        kRemoteUser,
        kLocalServer,
        kRemoteServer
    };

    enum class RegisterState
    {
        kNotRegistered = 0,
        kRegistering = 1,
        kRegistered = 2,
    };

    IClient() :
        type_(IClient::Type::kUnRegistered),
        register_state_(IClient::RegisterState::kNotRegistered),
        uuid_(IRC::UUIDGenerator::GetInstance().Generate()),
        local_server_(NULL),
        remote_server_(NULL)
    {};

    virtual ~IClient()
    {};

    IClient (IClient&& other) = default;

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

    /**
     * @brief Whether or not this client should be pinged.
     */
	virtual auto ShouldPing(time_t current_time) -> bool
	{
		return ((type_ == Type::kUnRegistered || type_ == Type::kLocalUser) &&
			current_time >= ping_time_);
	}
	virtual auto GetPingTime() -> time_t { return ping_time_; }
	virtual auto SetPingTime(time_t ping_time) -> void { ping_time_ = ping_time; }

	virtual auto RespondedToLastPing() -> bool { return last_ping_responded_ & 1; }
	virtual auto SetRespondedToLastPing(bool responded) -> void { last_ping_responded_ = responded; }
    
	virtual auto GetType() const -> IClient::Type { return type_; }
    virtual auto SetType(IClient::Type type) -> void { type_ = type; }
    virtual auto GetRegisterState() const -> IClient::RegisterState { return register_state_; }
    virtual auto SetRegisterState(IClient::RegisterState state) -> void { register_state_ = state; }
    virtual auto GetUUID() const -> const IRC::UUID& { return uuid_; };
    virtual auto GetPassword() const -> const std::string& { return password_; }
    virtual auto SetPassword(std::string password) -> void { password_ = password; }
    virtual auto GetNickname() const -> const std::string& { return nickname_; }
    virtual auto SetNickname(std::string nickname) -> void { nickname_ = nickname; }
    virtual auto GetUsername() const -> const std::string& { return username_; }
    virtual auto SetUsername(std::string username) -> void { username_ = username; }
    virtual auto GetRealname() const -> const std::string& { return realname_; }
    virtual auto SetRealname(std::string realname) -> void { realname_ = realname; }
    virtual auto GetLocalServer() const -> IServer* { return local_server_; }

	virtual auto GetPrefix() const -> const std::string& { return prefix_; }

	virtual auto SetHops(uint32_t hops) -> void { hops_ = hops; }
	virtual auto GetHops(void) const -> const uint32_t { return hops_; }

	virtual auto SetOurToken(uint32_t t) -> void { our_token_ = t; }
	virtual auto GetOurToken(void) const -> const uint32_t { return our_token_; }

	virtual auto SetTheirToken(uint32_t t) -> void { their_token_ = t; }
	virtual auto GetTheirToken(void) const -> const uint32_t { return their_token_; }

    IClient (IClient& other) = delete;
    IClient &operator =(IClient& other) = delete;
    IClient &operator= (IClient&& other) = delete;

    class Disconnected : public std::runtime_error
    {
    public:
        Disconnected(const char *msg) : std::runtime_error(msg) {}
    };

    protected:

	/**
	 * @brief The time (in seconds) when this client should be pinged for activity.
	 */
	time_t ping_time_;
	/**
	 * @brief Whether or not the client responded to the last ping (0 = false, 1 = true).
	 */
	unsigned int last_ping_responded_:1;

    Type type_;
    RegisterState register_state_;
    IRC::UUID uuid_;
	std::string prefix_;
    std::string password_;
    std::string nickname_;
    std::string username_;
    std::string realname_;
    IServer* local_server_; /* The server which is directly connected to us */
    IServer* remote_server_; /* The server that has the local user */

	uint32_t hops_;
	uint32_t our_token_;
	uint32_t their_token_;
};

#endif
