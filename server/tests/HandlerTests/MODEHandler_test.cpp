#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "Numerics.h"
#include "Utilities.h"
#include "MODEHandler.h"

#include "MockChannel.h"
#include "MockMessage.h"
#include "MockLocalUser.h"
#include "MockServerConfig.h"
#include "MockClientDatabase.h"
#include "MockChannelDatabase.h"

using ::testing::_;
using ::testing::Throw;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::ReturnRef;
using ::testing::StrictMock;

class MODETests : public ::testing::Test
{
    public:
	std::unique_ptr<MODEHandler> handler;

	StrictMock<MockChannel> mock_channel1;
    StrictMock<MockLocalUser> mock_localuser1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();

    std::string channel1_key;
    std::string channel1_name;

    MockMessage message1;
    std::vector<std::string> message_params;

    MockClientDatabase mock_client_database;
    MockChannelDatabase mock_channel_database;

    void SetUp() override
    {
		handler = std::make_unique<MODEHandler>(&mock_client_database, &mock_channel_database);

        channel1_key = "p@ssw0rd";
		channel1_name = "#channel1";

		mock_channel1.SetName(channel1_name);

        EXPECT_CALL(mock_localuser1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));

        EXPECT_CALL(mock_channel_database, GetChannel(channel1_name))
            .WillRepeatedly(Return(std::optional<IChannel*>(&mock_channel1)));
        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_localuser1)));

        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(message_params));

		EXPECT_CALL(mock_channel1, HasUser(uuid1))
			.WillRepeatedly(Return(true));
    }
};

TEST_F(MODETests, SuccessTest)
{
	// Arrange
    message_params.push_back(channel1_name);
	message_params.push_back("+kt-t");
	message_params.push_back(channel1_key);

	EXPECT_CALL(mock_channel1, HasOperator(uuid1))
		.WillRepeatedly(Return(true));

	EXPECT_CALL(mock_channel1, PushToLocal("MODE " + message_params[0] + " " +
		message_params[1] + " " + message_params[2]));

	// Act
    handler->Handle(message1);

	// Assert
	ASSERT_EQ(mock_channel1.GetKey(), channel1_key);
	ASSERT_EQ(mock_channel1.HasMode(ChannelMode::CM_TOPIC), false);
}

TEST_F(MODETests, FailureTest_WrongMode)
{
	// Arrange
    message_params.push_back(channel1_name);
	message_params.push_back("+gbnk");

	EXPECT_CALL(mock_channel1, HasOperator(uuid1))
		.WillRepeatedly(Return(true));

	EXPECT_CALL(mock_localuser1, Push(GetErrorMessage(ERR_UMODEUNKNOWNFLAG, std::string(1, 'g'))));

	// Act
    handler->Handle(message1);
}
