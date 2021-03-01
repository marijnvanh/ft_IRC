#ifndef __ILOCAL_USER_H__
#define __ILOCAL_USER_H__

#include "IUser.h"

class ILocalUser : public virtual IUser 
{
    public:

    ILocalUser()
    {
        type_ = IClient::Type::kLocalUser;
    };
    virtual ~ILocalUser() {};

    private:
};

#endif
