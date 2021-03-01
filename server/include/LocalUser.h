#ifndef __LOCAL_USER_H__
#define __LOCAL_USER_H__

#include "Client.h"
#include "User.h"
#include "ILocalUser.h"

class LocalUser : public ILocalUser, Client, User
{
    public:

    LocalUser() = delete;
    LocalUser(Client &&old_client) : Client(std::move(old_client)) {};
    ~LocalUser() {};

    private:
};

#endif
