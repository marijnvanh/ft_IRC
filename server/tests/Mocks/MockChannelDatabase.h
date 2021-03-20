#ifndef MOCK_CHANNEL_DATABASE_H__
#define MOCK_CHANNEL_DATABASE_H__

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IChannelDatabase.h"

class MockChannelDatabase : public IChannelDatabase {
    public:

    MOCK_METHOD(void, DeleteEmptyChannels, (), ());

    MOCK_METHOD(void, RemoveChannel, (std::string channel_name), ());
    MOCK_METHOD(IChannel*, AddChannel, (std::unique_ptr<IChannel> new_channel), ());
    MOCK_METHOD(std::optional<IChannel*>, GetChannel, (std::string channel_name), ());
	MOCK_METHOD(std::optional<IChannel*>, CreateChannel, (const std::string name, const std::string key, ChannelType type, ChannelMode mode), ());
	MOCK_METHOD(void, ForEachChannel, (std::function<void(IChannel*)> action), ());

};

#endif

