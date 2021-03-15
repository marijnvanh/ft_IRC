#ifndef __IREMOTE_USER_H__
#define __IREMOTE_USER_H__

#include "IUser.h"

class IRemoteUser : public virtual IUser 
{
    public:

    IRemoteUser()
    {
        type_ = IClient::Type::kRemoteUser;
    };
    virtual ~IRemoteUser() {};

    virtual auto GetServer() -> IServer* = 0;

    private:
};

#endif
