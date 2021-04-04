#ifndef __CHANNEL_DATABASE__
#define __CHANNEL_DATABASE__

#include <memory>
#include <string>
#include <unordered_map>

#include "Logger.h"
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
	auto CreateChannel(const std::string name, const std::string key,
		ChannelType type) -> std::optional<IChannel*> override;

    auto GetChannel(const std::string channel_name) -> std::optional<IChannel*> override;

	auto ForEachChannel(std::function<void(IChannel*)> action) -> void override;

    Logger logger;
private:
	std::unordered_map<std::string, std::unique_ptr<IChannel>> channels_;

};

#endif
