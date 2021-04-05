#ifndef __I_CHANNEL_H__
#define __I_CHANNEL_H__

#include <string>
#include <vector>
#include <memory>
#include <optional>

#include "IUser.h"

class IUser;

enum class ChannelMode:uint8_t
{
	/** +k: Channel key. */
	CM_KEY = 'k' - 65,
	/** +t: Channel topic. */
	CM_TOPIC = 't' - 65,
	/** +o: Channel operator. */
	CM_OPERATOR = 'o' - 65
};

class IChannel
{

public:

    enum ChannelType
    {
        kLocal = 0,
        kNetwork = 1
    };

	virtual ~IChannel() {};

	/**
     * @brief Push a message on to the send queue of all local users.
	 * 
     * @param irc_message The message to be send to all local users.
	 * @param except_uuid uuid that will be skipped
	 */
    virtual auto PushToLocal(const std::string &irc_message, std::optional<IRC::UUID> except_uuid) -> void = 0;

    virtual auto AddUser(IUser* new_user) -> void = 0;
	/**
     * @brief Attempts to remove a user from the channel with the given UUID key.
     * 
     * @returns Whether or not a user was actually removed.
     */
    virtual auto RemoveUser(IRC::UUID uuid) -> bool = 0;

	virtual auto HasUser(IRC::UUID uuid) -> bool = 0;
	virtual auto CountUsers() -> uint32_t = 0;
	virtual auto GetUserListAsString() -> const std::string = 0;

	virtual auto AddOperator(IRC::UUID uuid) -> bool = 0;
	virtual auto HasOperator(IRC::UUID uuid) -> bool = 0;
	virtual auto RemoveOperator(IRC::UUID uuid) -> bool = 0;

	virtual auto SetKey(std::string new_key) -> void { key_ = new_key; }
	virtual auto SetName(std::string new_name) -> void { name_ = new_name; }
	virtual auto SetType(ChannelType new_type) -> void { type_ = new_type; }
	virtual auto SetTopic(std::string new_topic) -> void { topic_ = new_topic; }

	virtual auto GetKey() const -> std::string { return key_; }
	virtual auto GetName() const -> std::string { return name_; }
	virtual auto GetType() const -> ChannelType { return type_; }
	virtual auto GetTopic() const -> std::string { return topic_; }

	/**
	 * @brief Mode specific functionality. Sets the given mode.
	 */
	virtual auto SetMode(ChannelMode mode_key, bool value) -> void
		{ mode_[(uint8_t)mode_key] = value; }

	/**
	 * @brief Mode specific functionality. Gets the the constant mode object.
	 */
	virtual auto GetMode() const -> std::bitset<64> { return mode_; }

	/**
	 * @brief Mode specific functionality. Returns the value of the given mode.
	 * 
	 * @returns Whether or not the given mod is set or unset.
	 */
	virtual auto HasMode(ChannelMode mode_key) const -> bool
		{ return mode_[(uint8_t)mode_key]; }

protected:
	std::string key_;
	std::string name_;

	std::string topic_;

	ChannelType type_;
	std::bitset<64> mode_;

};

#endif // __I_CHANNEL_H__
