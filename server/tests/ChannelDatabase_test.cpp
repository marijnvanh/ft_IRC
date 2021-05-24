#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <memory>
#include <optional>

#include "MockChannel.h"

#include "IChannel.h"
#include "MockUser.h"
#include "ChannelDatabase.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class ChannelDatabaseTests : public ::testing::Test
{

public:

    std::shared_ptr<ChannelDatabase> channel_database;

    void SetUp() override
    {
        channel_database = std::make_shared<ChannelDatabase>();
	}

};

TEST_F(ChannelDatabaseTests, CreateAndRemoveChannel)
{
	std::string channel_name = "TestChannel1";

    channel_database->CreateChannel(channel_name, std::string(), ChannelType::kLocal);

	auto channel = channel_database->GetChannel(channel_name);
    ASSERT_EQ((*channel)->GetName(), channel_name);

    channel_database->RemoveChannel(channel_name);

    ASSERT_EQ(channel_database->GetChannel(channel_name), std::nullopt);
}

TEST_F(ChannelDatabaseTests, DeleteEmptyChannels)
{
	// Setup
	std::string channel_name = "TestChannel1";
	std::string channel_name2 = "TestChannel2";

    channel_database->CreateChannel(channel_name, std::string(), ChannelType::kLocal);
	auto channel1 = channel_database->GetChannel(channel_name);
    ASSERT_EQ((*channel1)->GetName(), channel_name);

    channel_database->CreateChannel(channel_name2, std::string(), ChannelType::kLocal);
	auto channel2 = channel_database->GetChannel(channel_name2);
    ASSERT_EQ((*channel2)->GetName(), channel_name2);

	// Act
	MockUser user1;

	IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
	EXPECT_CALL(user1, GetUUID()).WillRepeatedly(ReturnRef(uuid1));

	(*channel2)->AddUser(&user1, false);

    channel_database->DeleteEmptyChannels();

	// Assert
    ASSERT_EQ(channel_database->GetChannel(channel_name), std::nullopt);
    ASSERT_EQ(*channel_database->GetChannel(channel_name2), channel2);
}
