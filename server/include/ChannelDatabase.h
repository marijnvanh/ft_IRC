#ifndef __CHANNEL_DATABASE__
#define __CHANNEL_DATABASE__

#include <memory>
#include <string>
#include <unordered_map>

#include "IChannel.h"
#include "IChannelDatabase.h"

class ChannelDatabase : public IChannelDatabase
{

public:
	ChannelDatabase();
	~ChannelDatabase();

	auto DeleteEmptyChannels() -> void override;

    auto RemoveChannel(std::string channel_name) -> void override;
    auto AddChannel(std::unique_ptr<IChannel> new_channel) -> IChannel* override;
	auto CreateChannel(std::string channel_name, ChannelType type,
		ChannelMode mode) -> IChannel* override;

    auto GetChannel(const std::string channel_name) -> std::optional<IChannel*> override;

private:
	std::unordered_map<std::string, std::unique_ptr<IChannel>> channels_;

};

#endif
