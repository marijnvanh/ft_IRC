#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "USERHandler.h"
#include "MockClient.h"
#include "MockMessage.h"
#include "MockClientDatabase.h"
#include "MockServerConfig.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::StrictMock;

class USERFromUserTests : public ::testing::Test
{
    public:
	std::unique_ptr<USERHandler> user_handler_;

    StrictMock<MockClient> mock_client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();

    MockMessage message1;
    std::vector<std::string> message_params;

    StrictMock<MockClientDatabase> mock_client_database;
    MockServerConfig mock_server_config;

    void SetUp() override
    {
        user_handler_ = std::make_unique<USERHandler>(&mock_server_config, &mock_client_database);
        
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

    EXPECT_CALL(mock_client_database, RegisterLocalUser(uuid1))
        .WillOnce(Return(&mock_client1));
    EXPECT_CALL(mock_client1, Push(_))
        .Times(1);
    user_handler_->Handle(message1);

    ASSERT_EQ(mock_client1.GetUsername(), "username");
    ASSERT_EQ(mock_client1.GetRealname(), "realname");
    ASSERT_EQ(mock_client1.GetState(), IClient::State::kUnRegistered);
}
