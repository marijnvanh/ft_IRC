#include "Channel.h"
#include "IClient.h"
#include "LocalUser.h"

Channel::Channel(std::string name, std::string key, ChannelType type, ChannelMode mode = ChannelMode::None)
{
	key_ = key;
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

auto Channel::RemoveUser(IRC::UUID uuid) -> bool
{
	size_t result = 0;
    
	result += local_users_.erase(uuid);
    result += remote_users_.erase(uuid);

	return (result > 0);
}

auto Channel::AddUser(IUser* new_user) -> void
{
    auto uuid = new_user->GetUUID();

	if (new_user->GetType() == IClient::Type::kLocalUser)
	{
    	local_users_.insert(std::make_pair(uuid, dynamic_cast<ILocalUser*>(new_user)));
	}
	else
	{
    	remote_users_.insert(std::make_pair(uuid, dynamic_cast<IRemoteUser*>(new_user)));	
	}
}

auto Channel::HasUser(IRC::UUID uuid) -> bool
{
	if (local_users_.find(uuid) != local_users_.end())
	{
		return (true);
	}

	if (remote_users_.find(uuid) != remote_users_.end())
	{
		return (true);
	}

	return (false);
}

auto Channel::CountUsers() -> uint32_t
{
	return (local_users_.size() + remote_users_.size());
}

auto Channel::GetUserListAsString() -> const std::string
{
	std::string result;

	for (auto it = local_users_.cbegin(); it != local_users_.cend();)
	{
		result += it->second->GetNickname();
		if (++it != local_users_.cend() || remote_users_.size() > 0)
		{
			result += ' ';
		}
	}

	for (auto it = remote_users_.cbegin(); it != remote_users_.cend();)
	{
		result += it->second->GetNickname();
		++it;

		if (it != remote_users_.cend())
		{
			result += ' ';
		}
	}

	return (result);
}
