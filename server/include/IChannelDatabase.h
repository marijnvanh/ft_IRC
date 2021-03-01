#ifndef __I_CHANNEL_DATABASE_H__
#define __I_CHANNEL_DATABASE_H__

#include <string>
#include <memory>

#include "IChannel.h"

class IChannelDatabase
{
public:

    virtual ~IChannelDatabase();	

    virtual auto RemoveChannel(std::string uuid) -> void = 0;
    virtual auto AddChannel(std::unique_ptr<IChannel> new_channel) -> void = 0;

    virtual auto GetChannel(std::string uuid) -> std::shared_ptr<IChannel> = 0;
    virtual auto FindChannel(const std::string &nickname) -> std::optional<std::shared_ptr<IChannel>> = 0;

};

#endif // __I_CHANNEL_DATABASE_H__
