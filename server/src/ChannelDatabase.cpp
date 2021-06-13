#include <optional>

#include "Channel.h"
#include "ChannelDatabase.h"

ChannelDatabase::ChannelDatabase() :
    logger("ChannelDatabase")
{}

ChannelDatabase::~ChannelDatabase()
{}

auto ChannelDatabase::RemoveChannel(std::string channel_name) -> void
{
    auto channel = GetChannel(channel_name);
    if (channel)
    {
        if ((*channel)->CountUsers() != 0)
            logger.Log(LogLevel::ERROR, "Deleting a channel that is not empty !!!");
    }
    else
    {
        logger.Log(LogLevel::ERROR, "Trying to delete a channel that does not exist !!!");
    }
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

auto ChannelDatabase::CreateChannel(const std::string name, const std::string key,
	ChannelType type = ChannelType::kLocal) -> std::optional<IChannel*>
{
	auto new_channel = std::make_unique<Channel>(name, key, type);

    logger.Log(LogLevel::INFO, "New channel <%s> with key <%s> created.", name.c_str(), key.c_str());

	return this->AddChannel(std::move(new_channel));
}

auto ChannelDatabase::AddChannel(std::unique_ptr<IChannel> new_channel) -> IChannel*
{
    auto channel_name = new_channel->GetName();
    auto ret = channels_.insert(std::make_pair(channel_name, std::move(new_channel)));

    if (ret.second == false)
		throw DuplicateChannel();
	return ret.first->second.get();
}

auto ChannelDatabase::GetChannel(std::string channel_name) -> std::optional<IChannel*>
{
	auto channel = channels_.find(channel_name);
	if (channel != channels_.end())
		return std::optional<IChannel*>(channel->second.get());

    return std::nullopt;
}

auto ChannelDatabase::ForEachChannel(std::function<void(IChannel*)> action) -> void
{
    for (auto it = channels_.begin(); it != channels_.end(); it++)
    {
        action(it->second.get());
    }	
}

auto ChannelDatabase::PurgeEmptyChannels(void) -> void
{
    for (auto it = channels_.begin(), next_it = it; it != channels_.end(); it = next_it)
    {
        ++next_it;

		if (it->second->CountUsers() > 0)
			continue ;
		
		logger.Log(LogLevel::INFO, "Purging channel %s, no users left.",
			it->second->GetName().c_str());
    	channels_.erase(it->first);
	}
}
