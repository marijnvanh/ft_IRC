#include "Channel.h"
#include "IClient.h"
#include "LocalUser.h"

Channel::Channel(std::string name, ChannelType type, ChannelMode mode = ChannelMode::None)
{
	name_ = name;
	type_ = type;

	mode_ = mode;
}

Channel::~Channel() { }

auto Channel::PushToAll(std::string irc_message) -> void
{
	this->PushToLocal(irc_message);
	this->PushToRemote(irc_message);
}

auto Channel::PushToLocal(std::string irc_message) -> void
{
    for (auto it = local_users_.cbegin(); it != local_users_.cend(); ++it)
    {
		it->second->Push(irc_message);
	}
}

auto Channel::PushToRemote(std::string irc_message) -> void
{
    for (auto it = remote_users_.cbegin(); it != remote_users_.cend(); ++it)
    {
		it->second->Push(irc_message);
	}
}

auto Channel::RemoveUser(std::string nickname) -> void
{
    local_users_.erase(nickname);
    remote_users_.erase(nickname);
}

auto Channel::AddUser(std::shared_ptr<IUser> new_user) -> void
{
    auto nickname = new_user->GetNickname();

	if (new_user->GetType() == IClient::Type::kLocalUser)
	{
    	local_users_.insert(std::make_pair(nickname, std::dynamic_pointer_cast<ILocalUser>(new_user)));
	}
	else
	{
    	remote_users_.insert(std::make_pair(nickname, std::dynamic_pointer_cast<IRemoteUser>(new_user)));	
	}
}

auto Channel::CountUsers() -> uint32_t
{
	return (local_users_.size() + remote_users_.size());
}
