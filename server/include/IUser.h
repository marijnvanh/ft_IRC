#ifndef __IUSER_H__
#define __IUSER_H__

#include "IClient.h"

class IUser : public virtual IClient 
{
    public:

    IUser()
    {
        state_ = IClient::State::kRegistered;
    };
    virtual ~IUser() {};

    private:
};

#endif
