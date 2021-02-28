#ifndef __REMOTE_USER_H__
#define __REMOTE_USER_H__

#include "Client.h"
#include "User.h"
#include "IRemoteUser.h"

class RemoteUser : public IRemoteUser, Client, User
{
    public:

    RemoteUser() = delete;
    RemoteUser(Client &&old_client) : Client(std::move(old_client)) {};
    virtual ~RemoteUser() {};

    private:
};

#endif
