#include "RemoteUser.h"

RemoteUser::RemoteUser( IServer* local_server, IServer* remote_server, std::string nickname, std::string username) : 
    logger("RemoteUser")
{
    local_server_ = local_server;
    remote_server_ = remote_server;
    nickname_ = nickname;
    username_ = username;
    type_ = IClient::Type::kRemoteUser;
    remote_server_->AddClient(this);

    logger.Log(LogLevel::INFO, "Remote user %s connected", GetNickname().c_str());
};
    
RemoteUser::~RemoteUser()
{
    remote_server_->RemoveClient(uuid_);
}

auto RemoteUser::Push(std::string irc_message) -> void
{
    local_server_->Push(irc_message);
}

/* Dummy implementation to meet IClient requirements */
auto RemoteUser::SendAll() -> void
{ ; }

/* Dummy implementation to meet IClient requirements */
auto RemoteUser::Receive() -> std::optional<std::string>
{ return std::nullopt; }
