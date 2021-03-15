#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "USERHandler.h"
#include "MockClient.h"
#include "MockMessage.h"
#include "MockClientDatabase.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class USERFromUserTests : public ::testing::Test
{
    public:
    MockClient mock_client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();

    MockMessage message1;
    std::vector<std::string> message_params;

    MockClientDatabase mock_client_database;

    void SetUp() override
    {

        EXPECT_CALL(mock_client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_client1)));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(message_params));
    }
};


TEST_F(USERFromUserTests, SuccessTest)
{
    message_params.push_back("username");
    message_params.push_back("unused");
    message_params.push_back("unused");
    message_params.push_back("realname");

    USERHandler(&mock_client_database, message1);

    ASSERT_EQ(mock_client1.GetUsername(), "username");
    ASSERT_EQ(mock_client1.GetRealname(), "realname");
    ASSERT_EQ(mock_client1.GetState(), IClient::State::kUnRegistered);
}
