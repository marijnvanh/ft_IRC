#include <optional>

#include "Channel.h"
#include "ChannelDatabase.h"

ChannelDatabase::ChannelDatabase() :
    channels_(std::unordered_map<std::string, std::shared_ptr<IChannel>>())
{}

ChannelDatabase::~ChannelDatabase()
{}

auto ChannelDatabase::RemoveChannel(std::string channel_name) -> void
{
    channels_.erase(channel_name);
}

auto ChannelDatabase::CreateChannel(std::string channel_name,
	ChannelType type = ChannelType::kLocal,
	ChannelMode mode = ChannelMode::None) -> std::shared_ptr<IChannel>
{
	auto new_channel = std::make_shared<Channel>(channel_name, type, mode);

	this->AddChannel(new_channel);

	return (new_channel);
}

auto ChannelDatabase::AddChannel(std::shared_ptr<IChannel> new_channel) -> void
{
    auto channel = new_channel;
    auto channel_name = channel->GetName();

    auto ret = channels_.insert(std::make_pair(channel_name, channel));

    if (ret.second == false)
	{
        throw DuplicateChannel();
	}
}

auto ChannelDatabase::GetChannel(std::string channel_name) -> std::shared_ptr<IChannel>
{
    auto channel = channels_.find(channel_name);
    if (channel == channels_.end())
	{
		throw ChannelNotFound();
	}

    return channel->second;
}

auto ChannelDatabase::FindChannel(const std::string &channel_name) -> std::optional<std::shared_ptr<IChannel>>
{
    for (auto it = channels_.begin(), next_it = it; it != channels_.end(); it = next_it)
    {
        ++next_it;
        if (it->second->GetName() == channel_name)
		{
            return std::optional<std::shared_ptr<IChannel>>(it->second);
		}
    }

    return std::nullopt;
}
