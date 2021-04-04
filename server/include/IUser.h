#ifndef __IUSER_H__
#define __IUSER_H__

#include "IClient.h"
#include "IChannel.h"

class IChannel;

class IUser : public virtual IClient 
{
    public:

    virtual ~IUser() {};
    virtual auto AddChannel(IChannel *channel) -> void = 0;
    virtual auto RemoveChannel(const std::string &channel_name) -> void = 0;
    virtual auto RemoveUserFromAllChannels() -> void = 0;

    private:
};

#endif
