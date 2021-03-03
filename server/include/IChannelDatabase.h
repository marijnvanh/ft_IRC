#ifndef __I_CHANNEL_DATABASE_H__
#define __I_CHANNEL_DATABASE_H__

#include <string>
#include <memory>

#include "IChannel.h"

using ChannelType = IChannel::ChannelType;

class IChannelDatabase
{
public:

    virtual ~IChannelDatabase() {};

	virtual auto DeleteEmptyChannels() -> void = 0;

    virtual auto RemoveChannel(std::string channel_name) -> void = 0;
    virtual auto AddChannel(std::shared_ptr<IChannel> new_channel) -> void = 0;
    virtual auto CreateChannel(std::string channel_name, ChannelType type, ChannelMode mode) -> std::shared_ptr<IChannel> = 0;

    virtual auto GetChannel(const std::string channel_name) -> std::optional<std::shared_ptr<IChannel>> = 0;

	class ChannelNotFound : public std::runtime_error
    {
    public:
        ChannelNotFound() : std::runtime_error("Channel not found in Database") {}
    };

    class DuplicateChannel : public std::runtime_error
    {
    public:
        DuplicateChannel() : std::runtime_error("Channel already in database") {}
    };

};

#endif // __I_CHANNEL_DATABASE_H__
