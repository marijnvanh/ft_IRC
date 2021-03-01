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

	// Get channel
	// Add channel
	// Remove channel

private:
	std::unordered_map<std::string, std::shared_ptr<IChannel>> channels_;

};

#endif
