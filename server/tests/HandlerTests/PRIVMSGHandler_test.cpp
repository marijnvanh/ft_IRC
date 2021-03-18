#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockMessage.h"
#include "MockClientDatabase.h"
#include "ChannelDatabase.h"
#include "MockLocalUser.h"
#include "PRIVMSGHandler.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class PRIVMSGTests : public ::testing::Test
{
    public:
    MockLocalUser mock_localuser1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    
    MockLocalUser mock_localuser2;
    std::string localuser_name2;

    MockMessage message1;
    std::vector<std::string> message_params;

    MockClientDatabase mock_client_database;
    ChannelDatabase mock_channel_database; //TODO make mock

    void SetUp() override
    {
        localuser_name2 = "nickname2";

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

TEST_F(PRIVMSGTests, SuccessTest)
{
    PRIVMSGHandler PRIVMSG_handler(&mock_client_database, &mock_channel_database);
    message_params.push_back(localuser_name2);
    message_params.push_back("message content");

    EXPECT_CALL(mock_localuser2, Push(": PRIVMSG nickname2 :message content")); //TODO fix this

    PRIVMSG_handler.Handle(message1);
}
