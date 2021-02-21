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
    std::shared_ptr<IRC::Mutex<IClient>> mock_client_shared1;
    std::unique_ptr<MockClient> mock_client_unique1;
    MockClient *mock_client1;

    MockMessage message1;
    std::vector<std::string> message_params;

    std::shared_ptr<MockClientDatabase> mock_client_database_shared;
    MockClientDatabase *mock_client_database;

    void SetUp() override
    {
        mock_client_unique1 = std::make_unique<MockClient>();
        mock_client1 = mock_client_unique1.get();
        mock_client_shared1 = std::make_shared<IRC::Mutex<IClient>>(std::move(mock_client_unique1));

        mock_client_database_shared = std::make_shared<MockClientDatabase>();
        mock_client_database = mock_client_database_shared.get();
    }
};


TEST_F(USERFromUserTests, SuccessTest)
{
    EXPECT_CALL(message1, GetClient())
        .WillRepeatedly(Return(mock_client_shared1));
    EXPECT_CALL(message1, GetParams())
        .WillRepeatedly(ReturnRef(message_params));
    message_params.push_back("username");
    message_params.push_back("unused");
    message_params.push_back("unused");
    message_params.push_back("realname");

    mock_client_shared1->Take()->SetNickname("nickname");

    USERHandler(mock_client_database_shared, message1);

    ASSERT_EQ(mock_client1->GetUsername(), "username");
    ASSERT_EQ(mock_client1->GetRealname(), "realname");
    ASSERT_EQ(mock_client1->GetState(), IClient::State::kRegistered);
}
