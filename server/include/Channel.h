#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "IChannel.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"

class Channel : public IChannel
{
public:
	Channel(std::string name, ChannelType type, ChannelMode mode);
	~Channel();

	auto PushToAll(std::string irc_message) -> void override;
	auto PushToLocal(std::string irc_message) -> void override;
	auto PushToRemote(std::string irc_message) -> void override;

    auto RemoveUser(std::string nickname) -> void override;
    auto AddUser(std::shared_ptr<IUser> new_user) -> void override;

private:
	std::unordered_map<std::string, std::shared_ptr<ILocalUser>> local_users_;
	std::unordered_map<std::string, std::shared_ptr<IRemoteUser>> remote_users_;

};

#endif
