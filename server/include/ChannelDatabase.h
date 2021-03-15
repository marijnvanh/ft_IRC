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
    auto AddChannel(std::shared_ptr<IChannel> new_channel) -> void override;
	auto CreateChannel(std::string channel_name,
		ChannelType type,
		ChannelMode mode) -> std::shared_ptr<IChannel> override;

    auto GetChannel(const std::string channel_name) -> std::optional<std::shared_ptr<IChannel>> override;

private:
	std::unordered_map<std::string, std::shared_ptr<IChannel>> channels_;

};

#endif
