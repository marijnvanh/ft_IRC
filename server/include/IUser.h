#ifndef __IUSER_H__
#define __IUSER_H__

#include <bitset>

#include "IClient.h"

enum UserMode
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

    IUser()
    {
        state_ = IClient::State::kRegistered;
    };
    virtual ~IUser() {};

	/**
	 * @brief Mode specific functionality. Sets the given mode.
	 */
	virtual auto SetMode(UserMode mode_key, bool value) -> void
		{ mode_[mode_key] = value; }

	/**
	 * @brief Mode specific functionality. Gets the the constant mode object.
	 */
	virtual auto GetMode() const -> std::bitset<64> { return mode_; }

	/**
	 * @brief Mode specific functionality. Returns the value of the given mode.
	 * 
	 * @returns Whether or not the given mod is set or unset.
	 */
	virtual auto HasMode(UserMode mode_key) const -> bool { return mode_[mode_key]; }

    private:
	
	std::bitset<64> mode_;
};

#endif
