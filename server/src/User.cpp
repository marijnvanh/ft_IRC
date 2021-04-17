#include "User.h"
#include "IServer.h"
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
    channels_.clear();
}

auto User::GetChannels() -> std::unordered_map<std::string, IChannel*>&
{
	return (this->channels_);
}

/*    NICK <nickname> <hopcount> <username> <host> <servertoken> <umode> <realname>
        NICK new_nick 1 username irc.codam.net 34 +i :Christophe Kalt */
auto User::GenerateNickMessage(const std::string &this_server_name) const -> std::string
{
    auto nick_message = "NICK " + nickname_ + " 1 " + username_ + " ";
    if (type_ == IClient::Type::kLocalUser)
        nick_message = nick_message + this_server_name;
    else
        nick_message = nick_message + remote_server_->GetServerName();

    nick_message = nick_message + " ignored ignored :" + realname_;

    return nick_message;
}
