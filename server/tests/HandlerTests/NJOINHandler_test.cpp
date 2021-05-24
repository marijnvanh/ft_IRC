#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "NJOINHandler.h"
#include "MockChannel.h"
#include "MockMessage.h"
#include "MockLocalUser.h"
#include "MockServer.h"
#include "MockClientDatabase.h"
#include "MockChannelDatabase.h"
#include "MockServerConfig.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::StrictMock;

class NJOINTests : public ::testing::Test
{
    public:
	std::unique_ptr<NJOINHandler> njoin_handler_;

    StrictMock<MockChannel> mock_channel1;
    std::string channel1_name;

    MockServer mock_localserver1;
    IRC::UUID uuid3 = IRC::UUIDGenerator::GetInstance().Generate();

    MockLocalUser mock_localuser1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    std::string nickname1;
    MockLocalUser mock_localuser2;
    IRC::UUID uuid2 = IRC::UUIDGenerator::GetInstance().Generate();
    std::string nickname2;


    MockMessage message1;
    std::vector<std::string> message_params;

    MockClientDatabase mock_client_database;
    MockChannelDatabase mock_channel_database;
    MockServerConfig mock_server_config;


    void SetUp() override
    {
		njoin_handler_ = std::make_unique<NJOINHandler>(&mock_server_config, &mock_client_database, &mock_channel_database);

		channel1_name = "#channel1";
		nickname1 = "nickname1";
		nickname2 = "nickname2";

        mock_localserver1.SetType(IClient::Type::kLocalServer);

		mock_channel1.SetName(channel1_name);

        EXPECT_CALL(mock_localuser1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));        
        EXPECT_CALL(mock_localuser2, GetUUID())
            .WillRepeatedly(ReturnRef(uuid2));
        
        EXPECT_CALL(mock_localuser1, Push(_))
            .WillRepeatedly(Return());

        EXPECT_CALL(mock_client_database, GetClient(uuid3))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_localserver1)));
        EXPECT_CALL(mock_client_database, GetUser(nickname1))
            .WillRepeatedly(Return(std::optional<IUser*>(&mock_localuser1)));
        EXPECT_CALL(mock_client_database, GetUser(nickname2))
            .WillRepeatedly(Return(std::optional<IUser*>(&mock_localuser2)));

        // EXPECT_CALL(mock_channel_database, GetChannel(channel1_name))
        //     .WillRepeatedly(Return(std::optional<IChannel*>(&mock_channel1)));

        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid3));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(message_params));
        EXPECT_CALL(message1, GetPrefix())
            .WillRepeatedly(Return("Servername"));
    }

	void TearDown() override
	{}
};

TEST_F(NJOINTests, SuccessTest)
{
    message_params.push_back(channel1_name);
    message_params.push_back("@@" + nickname1 + ",@" + nickname2);

	EXPECT_CALL(mock_channel_database, GetChannel(channel1_name))
			.WillOnce(Return(std::nullopt));
	EXPECT_CALL(mock_channel_database, CreateChannel(channel1_name, "", IChannel::kNetwork))
		.WillOnce(Return(std::optional<IChannel*>(&mock_channel1)));
	EXPECT_CALL(mock_client_database, BroadcastToLocalServers(_, std::optional{uuid3}))
        .WillOnce(Return());

    EXPECT_CALL(mock_channel1, HasUser(uuid1))
        .WillOnce(Return(false));
    EXPECT_CALL(mock_channel1, HasUser(uuid2))
        .WillOnce(Return(false));
    EXPECT_CALL(mock_channel1, PushToLocal(_, _))
        .Times(2)
        .WillRepeatedly(Return());

	EXPECT_CALL(mock_channel1, AddUser(&mock_localuser1, false));
	EXPECT_CALL(mock_channel1, AddUser(&mock_localuser2, true));

    njoin_handler_->SafeHandle(message1);
}
