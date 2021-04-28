#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "KILLHandler.h"
#include "MockChannel.h"
#include "MockMessage.h"
#include "MockLocalUser.h"
#include "MockClientDatabase.h"
#include "MockChannelDatabase.h"
#include "MockServerConfig.h"

using ::testing::Throw;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::TypedEq;
using ::testing::ReturnRef;
using ::testing::_;

class KILLTests : public ::testing::Test
{
    public:
	std::unique_ptr<KILLHandler> handler;

	MockChannel mock_channel1;
    MockLocalUser mock_localuser1;
    MockLocalUser mock_localuser2;

	std::string user1_nickname = "NICK_01";
	std::string user2_nickname = "NICK_02";

    IRC::UUID local_user1_uuid = IRC::UUIDGenerator::GetInstance().Generate();
    IRC::UUID local_user2_uuid = IRC::UUIDGenerator::GetInstance().Generate();

    MockMessage message1;
    std::vector<std::string> message_params;

    MockClientDatabase mock_client_database;
    MockChannelDatabase mock_channel_database;
    MockServerConfig mock_server_config;

    void SetUp() override
    {
		handler = std::make_unique<KILLHandler>(&mock_server_config, &mock_client_database);

        EXPECT_CALL(mock_localuser1, GetUUID())
            .WillRepeatedly(ReturnRef(local_user1_uuid));
        EXPECT_CALL(mock_localuser2, GetUUID())
            .WillRepeatedly(ReturnRef(local_user2_uuid));
        
        EXPECT_CALL(mock_localuser1, Push(_))
            .WillRepeatedly(Return());

        EXPECT_CALL(mock_client_database, GetClient(local_user1_uuid))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_localuser1)));
        EXPECT_CALL(mock_client_database, GetUser(user2_nickname))
            .WillRepeatedly(Return(std::optional<IUser*>(&mock_localuser2)));

        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(local_user1_uuid));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(message_params));
    }
};

TEST_F(KILLTests, KILLLocalUserSuccess)
{
	std::string kill_message = "DIE YOU REMOTE USER";
    message_params.push_back(user2_nickname);
    message_params.push_back(kill_message);

	mock_localuser1.SetMode(UserMode::UM_OPERATOR, true);

	EXPECT_CALL(mock_client_database, DisconnectClient(local_user2_uuid,
		TypedEq<std::optional<std::string>>(std::make_optional<std::string>(kill_message))));

    handler->Handle(message1);
}
