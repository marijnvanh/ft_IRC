#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include "IChannel.h"
#include "ILocalUser.h"
#include "IRemoteUser.h"
#include "Logger.h"

class Channel : public IChannel
{
public:
	Channel(std::string name, std::string key, ChannelType type);
	~Channel();

	auto PushToLocal(const std::string &irc_message, std::optional<IRC::UUID> except_uuid) -> void override;

	auto AddUser(IUser* new_user, bool is_operator) -> void override;
	auto HasUser(IRC::UUID uuid) -> bool override;
	auto RemoveUser(IRC::UUID uuid) -> bool override;

	auto CountUsers() -> uint32_t override;
	auto GetUserListAsString(char delim) -> const std::string override;

	auto AddOperator(IRC::UUID uuid) -> bool override;
	auto HasOperator(IRC::UUID uuid) -> bool override;
	auto RemoveOperator(IRC::UUID uuid) -> bool override;

private:
	std::unordered_map<IRC::UUID, ILocalUser*> local_users_;
	std::unordered_map<IRC::UUID, IRemoteUser*> remote_users_;

	std::vector<IRC::UUID> operators_;
	Logger logger;
};

#endif
