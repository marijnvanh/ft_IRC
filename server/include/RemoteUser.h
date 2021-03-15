#ifndef __REMOTE_USER_H__
#define __REMOTE_USER_H__

#include "Client.h"
#include "User.h"
#include "IRemoteUser.h"
#include "IServer.h"

class RemoteUser : public IRemoteUser, Client, User
{
    public:

    RemoteUser() = delete;
    RemoteUser(Client &&old_client, IServer* server);
    ~RemoteUser();

    auto GetServer() -> IServer* override;
    private:
    IServer* server_;
};

#endif
