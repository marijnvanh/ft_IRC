#ifndef __I_CHANNEL_H__
#define __I_CHANNEL_H__

#include <string>
#include <vector>
#include <memory>

#include "IUser.h"

class IChannel
{

public:

    enum ChannelType
    {
        kLocal = 0,
        kNetwork = 1
    };

	virtual ~IChannel();

	/**
     * @brief Push a message on to the send queue of all connected users.
     * 
     * @param irc_message The message to be send to all connected users.
     */
    virtual auto PushToAll(std::string irc_message) -> void = 0;

    virtual auto RemoveUser(std::string nickname) -> void = 0;
    virtual auto AddUser(std::unique_ptr<IUser> new_user) -> void = 0;

	virtual auto SetName(std::string new_name) -> void { name_ = new_name; }
	virtual auto SetMode(ChannelMode new_mode) -> void { mode_ = new_mode; }
	virtual auto SetType(ChannelType new_type) -> void { type_ = new_type; }

	virtual auto GetName() const -> std::string { return name_; }
	virtual auto GetMode() const -> ChannelMode { return mode_; }
	virtual auto GetType() const -> ChannelType { return type_; }

	virtual auto ClearMode() -> void { mode_ = ChannelMode::None; }
	virtual auto HasMode(ChannelMode mode) -> bool { return mode_ & mode; }

protected:
	std::string name_;

	ChannelMode mode_;
	ChannelType type_;

};

enum ChannelMode : uint32_t
{
	None				= 1 << 0,
	Private				= 1 << 1, // Private channel.
	Secret				= 1 << 2, // Secret channel.
	InviteOnly			= 1 << 3, // Invite-only channel.
	OperatorTopic		= 1 << 4, // Topic settable by channel operator only.
	NoOutsideMessages	= 1 << 5, // No messages to channel from clients on the outside.
	Moderated			= 1 << 6  // Moderated channel.
};

inline ChannelMode operator | (ChannelMode lhs, ChannelMode rhs)
{
	return static_cast<ChannelMode>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}
    
inline ChannelMode& operator |= (ChannelMode& lhs, ChannelMode rhs)
{
    lhs = lhs | rhs;
    return lhs;
}


#endif // __I_CHANNEL_H__
