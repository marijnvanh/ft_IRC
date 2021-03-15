#include <optional>

#include "Channel.h"
#include "ChannelDatabase.h"

ChannelDatabase::ChannelDatabase()
{}

ChannelDatabase::~ChannelDatabase()
{}

auto ChannelDatabase::RemoveChannel(std::string channel_name) -> void
{
    channels_.erase(channel_name);
}

auto ChannelDatabase::DeleteEmptyChannels() -> void
{
    for (auto it = channels_.begin(), next_it = it; it != channels_.end(); it = next_it)
    {
        ++next_it;
        if (it->second->CountUsers() == 0)
		{
			this->channels_.erase(it->first);
		}
    }	
}

auto ChannelDatabase::CreateChannel(std::string channel_name,
	ChannelType type = ChannelType::kLocal,
	ChannelMode mode = ChannelMode::None) -> IChannel*
{
	auto new_channel = std::make_unique<Channel>(channel_name, type, mode);

	return this->AddChannel(std::move(new_channel));
}

auto ChannelDatabase::AddChannel(std::unique_ptr<IChannel> new_channel) -> IChannel*
{
    auto channel_name = new_channel->GetName();
    auto ret = channels_.insert(std::make_pair(channel_name, std::move(new_channel)));

    if (ret.second == false)
	{
        throw DuplicateChannel();
		return NULL; // For some reason, not returning something throws a compile-time error.
	}
	return ret.first->second.get();
}

auto ChannelDatabase::GetChannel(std::string channel_name) -> std::optional<IChannel*>
{
	auto channel = channels_.find(channel_name);
	if (channel != channels_.end())
		return std::optional<IChannel*>(channel->second.get());

    return std::nullopt;
}
