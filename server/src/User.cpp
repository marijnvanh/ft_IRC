#include "User.h"

User::~User()
{}

auto User::AddChannel(IChannel *channel) -> void
{
    auto channel_name = channel->GetName();
    channels_.insert(std::make_pair(channel_name, channel));
}

auto User::RemoveChannel(const std::string &channel_name) -> void
{
    channels_.erase(channel_name);
}

auto User::RemoveUserFromAllChannels() -> void
{
	for (auto it = channels_.cbegin(); it != channels_.cend(); it++)
	{
        it->second->RemoveUser(GetUUID());
	}
}
