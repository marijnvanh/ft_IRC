#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "JOINHandler.h"
#include "MockChannel.h"
#include "MockMessage.h"
#include "MockLocalUser.h"
#include "MockClientDatabase.h"
#include "MockChannelDatabase.h"
#include "MockServerConfig.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class JOINTests : public ::testing::Test
{
    public:
	JOINHandler *handler;

	MockChannel mock_channel1;
    MockLocalUser mock_localuser1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();

    std::string channel1_key;
    std::string channel1_name;

    MockMessage message1;
    std::vector<std::string> message_params;

    MockClientDatabase mock_client_database;
    MockChannelDatabase mock_channel_database;
    MockServerConfig mock_server_config;


    void SetUp() override
    {
		handler = new JOINHandler(&mock_server_config, &mock_client_database, &mock_channel_database);

        channel1_key = "p@ssw0rd";
		channel1_name = "#channel1";

		mock_channel1.SetKey(channel1_key);
		mock_channel1.SetName(channel1_name);

        EXPECT_CALL(mock_localuser1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        
        EXPECT_CALL(mock_localuser1, Push(_))
            .WillRepeatedly(Return());

        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_localuser1)));

        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(message_params));
    }

	void TearDown() override
	{
		delete(handler);
	}
};

TEST_F(JOINTests, SuccessTest)
{
    message_params.push_back(channel1_name);
    message_params.push_back(channel1_key);

	EXPECT_CALL(mock_channel_database, GetChannel(channel1_name))
			.WillOnce(Return(std::nullopt));
	EXPECT_CALL(mock_channel_database, CreateChannel(channel1_name, channel1_key, IChannel::kLocal))
		.WillOnce(Return(std::optional<IChannel*>(&mock_channel1)));

    EXPECT_CALL(mock_channel1, HasUser(_))
        .WillOnce(Return(false));

	EXPECT_CALL(mock_channel1, AddUser(&mock_localuser1, true));

    handler->Handle(message1);
}
