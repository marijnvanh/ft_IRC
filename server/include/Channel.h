#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "IChannel.h"

class Channel : public IChannel
{
public:
	Channel(std::string name, ChannelType type);
	~Channel();

	auto PushToAll(std::string irc_message) -> void override;

    auto RemoveUser(std::string nickname) -> void override;
    auto AddUser(std::unique_ptr<IUser> new_user) -> void override;

private:
	std::unordered_map<std::string, std::shared_ptr<IUser>> connected_users_;

};

#endif
