#include "RemoteUser.h"

RemoteUser::RemoteUser( IServer* local_server, IServer *remote_server, 
    const std::string &nickname, const std::string &username, const std::string &realname) :
    logger("RemoteUser")
{
    local_server_ = local_server;
    remote_server_ = remote_server;
    nickname_ = nickname;
    username_ = username;
    realname_ = realname;
    type_ = IClient::Type::kRemoteUser;
    remote_server_->AddClient(this);
	CachePrefix(remote_server_->GetServerName());

    logger.Log(LogLevel::INFO, "Remote user %s connected", GetNickname().c_str());
};
    
RemoteUser::~RemoteUser()
{
    remote_server_->RemoveClient(uuid_);
}

auto RemoteUser::Push(std::string irc_message) -> void
{
    local_server_->Push(":" + prefix_ + " " + irc_message);
}

/* Dummy implementation to meet IClient requirements */
auto RemoteUser::SendAll() -> void
{ ; }

/* Dummy implementation to meet IClient requirements */
auto RemoteUser::Receive() -> std::optional<std::string>
{ return std::nullopt; }
