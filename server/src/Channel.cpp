#include "Channel.h"
#include "IClient.h"
#include "LocalUser.h"

Channel::Channel(std::string name, std::string key, ChannelType type) :
	logger("Channel")
{
	key_ = key;
	name_ = name;

	type_ = type;

	// Auto-set password mode if key is enabled.
	this->SetMode(ChannelMode::CM_KEY, !key_.empty());
}

Channel::~Channel() { }

auto Channel::PushToLocal(const std::string &irc_message, std::optional<IRC::UUID> except_uuid) -> void
{
    for (auto it = local_users_.cbegin(); it != local_users_.cend(); ++it)
    {
	    if (!except_uuid || it->second->GetUUID() != *except_uuid)
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

auto Channel::AddUser(IUser* new_user, bool is_operator) -> void
{
    auto uuid = new_user->GetUUID();
	if (HasUser(uuid)) {
		logger.Log(LogLevel::ERROR, "Failed to add user, user %s is already part of channel %s.", new_user->GetNickname().c_str(), name_.c_str());
		return ;
	}

	if (new_user->GetType() == IClient::Type::kLocalUser)
	{
    	local_users_.insert(std::make_pair(uuid, dynamic_cast<ILocalUser*>(new_user)));
	}
	else
	{
    	remote_users_.insert(std::make_pair(uuid, dynamic_cast<IRemoteUser*>(new_user)));	
	}
	new_user->AddChannel(this);

	if (is_operator)
	{
		AddOperator(uuid);
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

auto Channel::GetUserListAsString(char delim) -> const std::string
{
	std::string result;

	for (auto it = local_users_.cbegin(); it != local_users_.cend();)
	{
		if (HasOperator(it->second->GetUUID()))
			result += "@";
		result += it->second->GetNickname();
		if (++it != local_users_.cend() || remote_users_.size() > 0)
		{
			result += delim;
		}
	}

	for (auto it = remote_users_.cbegin(); it != remote_users_.cend();)
	{
		if (HasOperator(it->second->GetUUID()))
			result += "@";
		result += it->second->GetNickname();
		if (++it != remote_users_.cend())
		{
			result += delim;
		}
	}

	return (result);
}

auto Channel::AddOperator(IRC::UUID uuid) -> bool
{
	for (auto it = operators_.cbegin(); it != operators_.cend(); ++it)
	{
		if (*it == uuid)
		{
			return (false);
		}
	}

	operators_.push_back(uuid);
	return (true);
}

auto Channel::HasOperator(IRC::UUID uuid) -> bool
{
	for (auto it = operators_.cbegin(); it != operators_.cend(); ++it)
	{
		if (*it == uuid)
		{
			return (true);
		}
	}
	return (false);
}

auto Channel::RemoveOperator(IRC::UUID uuid) -> bool
{
	for (auto it = operators_.cbegin(); it != operators_.cend(); ++it)
	{
		if (*it == uuid)
		{
			operators_.erase(it);
			return (true);
		}
	}

	return (false);
}
