#include "Channel.h"

Channel::Channel(std::string name, ChannelType type)
	: name_(name), type_(type)
{

}

Channel::~Channel()
{
	connected_users_.clear();
}

auto Channel::PushToAll(std::string irc_message) -> void
{

}

auto Channel::RemoveUser(std::string nickname) -> void
{
    connected_users_.erase(nickname);	
}

auto Channel::AddUser(std::unique_ptr<IUser> new_user) -> void
{
    auto nickname = new_user->GetNickname();
    auto user = std::make_shared<IUser>(std::move(new_user));

    auto ret = connected_users_.insert(std::make_pair(nickname, user));
}
