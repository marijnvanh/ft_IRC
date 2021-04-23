#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockMessage.h"
#include "MockClientDatabase.h"
#include "MockLocalUser.h"
#include "MockChannelDatabase.h"
#include "MockChannel.h"
#include "NAMESHandler.h"
#include "MockServerConfig.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class NAMESHandlerTests : public ::testing::Test
{
    public:
    MockLocalUser mock_localuser1;
    std::string localuser_name1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    
    MockLocalUser mock_localuser2;
    std::string localuser_name2;

    MockMessage message1;
    std::vector<std::string> message_params;

    MockClientDatabase mock_client_database;
    MockChannelDatabase mock_channel_database;
    MockServerConfig mock_server_config;

    MockChannel mock_channel;
    std::string mock_channel_name;

    void SetUp() override
    {
        localuser_name2 = "nickname2";
        localuser_name1 = "nickname1";
        mock_channel_name = "#channel1";
        mock_channel.SetName(mock_channel_name);

        EXPECT_CALL(mock_localuser1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_localuser1)));
        EXPECT_CALL(mock_client_database, GetUser(localuser_name2))
            .WillRepeatedly(Return(std::optional<IUser*>(&mock_localuser2)));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(message_params));
    }
};

TEST_F(NAMESHandlerTests, NAMESWithChannelList)
{
    NAMESHandler NAMES_handler(&mock_server_config, &mock_client_database, &mock_channel_database);
    message_params.push_back(mock_channel_name);

    EXPECT_CALL(mock_channel_database, GetChannel(mock_channel_name))
        .WillOnce(Return(std::optional<IChannel*>(&mock_channel)));
    EXPECT_CALL(mock_channel, GetUserListAsString())
        .WillOnce(Return(localuser_name1));

    EXPECT_CALL(mock_localuser1, Push("353 #channel1 :nickname1")); //TODO fix this
    EXPECT_CALL(mock_localuser1, Push("366 #channel1 :End of /NAMES list")); //TODO fix this

    NAMES_handler.Handle(message1);
}

TEST_F(NAMESHandlerTests, NAMESCommandForAllChannels)
{
    NAMESHandler NAMES_handler(&mock_server_config, &mock_client_database, &mock_channel_database);

    EXPECT_CALL(mock_channel_database, ForEachChannel(_));

    NAMES_handler.Handle(message1);
}
