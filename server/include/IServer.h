#ifndef __ISERVER_H__
#define __ISERVER_H__

#include "IClient.h"

class IServer : public virtual IClient 
{
    public:

    IServer()
    {
        state_ = IClient::State::kRegistered;
        type_ = IClient::Type::kServer;
    };
    virtual ~IServer() {};

    private:
};

#endif
