#include "RemoteUser.h"

RemoteUser::RemoteUser(Client &&old_client, IServer* server) : Client(std::move(old_client)), server_(server)
{
    type_ = IClient::Type::kRemoteUser;
    server_->AddClient(this);
};
    
    
RemoteUser::~RemoteUser()
{
    server_->RemoveClient(uuid_);
}

auto RemoteUser::GetServer() -> IServer*
{
    return (server_);
}
