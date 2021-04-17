#ifndef __IUSER_H__
#define __IUSER_H__

#include <string>
#include <bitset>

#include "IClient.h"
#include "IChannel.h"

class IChannel;

enum class UserMode:uint8_t
{
	/** +s: Server notice mask */
	UM_SNOMASK = 's' - 65,
	/** +w: WALLOPS */
	UM_WALLOPS = 'w' - 65,
	/** +i: Invisible */
	UM_INVISIBLE = 'i' - 65,
	/** +o: Operator */
	UM_OPERATOR = 'o' - 65
};

class IUser : public virtual IClient 
{
    public:

    virtual ~IUser() {};
    virtual auto AddChannel(IChannel *channel) -> void = 0;
    virtual auto RemoveChannel(const std::string &channel_name) -> void = 0;
    virtual auto RemoveUserFromAllChannels() -> void = 0;
	virtual auto GetChannels() -> std::unordered_map<std::string, IChannel*>& = 0;
	virtual auto GenerateNickMessage(const std::string &this_server_name) const -> std::string = 0;

	/**
	 * @brief Mode specific functionality. Sets the given mode.
	 */
	virtual auto SetMode(UserMode mode_key, bool value) -> void
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
	virtual auto HasMode(UserMode mode_key) const -> bool
		{ return mode_[(uint8_t)mode_key]; }

    private:
	
	std::bitset<64> mode_;
};

#endif
