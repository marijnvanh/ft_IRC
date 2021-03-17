#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "IChannel.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"

class Channel : public IChannel
{
public:
	Channel(std::string name, std::string key, ChannelType type, ChannelMode mode);
	~Channel();

	auto PushToAll(std::string irc_message) -> void override;
	auto PushToLocal(std::string irc_message) -> void override;
	auto PushToRemote(std::string irc_message) -> void override;

    auto AddUser(IUser* new_user) -> void override;
    auto RemoveUser(IRC::UUID uuid) -> void override;

	auto CountUsers() -> uint32_t override;

private:
	std::unordered_map<IRC::UUID, ILocalUser*> local_users_;
	std::unordered_map<IRC::UUID, IRemoteUser*> remote_users_;

};

#endif
