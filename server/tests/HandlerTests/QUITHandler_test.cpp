#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "QUITHandler.h"
#include "MockClientDatabase.h"
#include "MockClient.h"
#include "MockMessage.h"
#include "ILocalUser.h"
#include "MockLocalUser.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TypedEq;
using ::testing::_;
using ::testing::StrictMock;

class QUITTests : public ::testing::Test
{
    public:
    // std::shared_ptr<IClient> server_client_shared1;
    // std::unique_ptr<MockClient> server_client_unique1;
    // MockClient *server_client1;
    // MockMessage server_message1;
    // std::vector<std::string> server_message_params1;

    std::unique_ptr<QUITHandler> quit_handler_;

    StrictMock<MockLocalUser> local_user1;
    MockMessage local_user_message1;
    std::string local_user_nickname1;
    std::vector<std::string> local_user_message_params1;
    IRC::UUID local_user1_uuid = IRC::UUIDGenerator::GetInstance().Generate();

    StrictMock<MockClient> base_client1;
    MockMessage base_message1;
    std::string base_nickname1;
    std::vector<std::string> base_message_params1;
    IRC::UUID base_client1_uuid = IRC::UUIDGenerator::GetInstance().Generate();

    StrictMock<MockClientDatabase> mock_client_database;

    void SetUp() override
    {
        quit_handler_ = std::make_unique<QUITHandler>(&mock_client_database);
        // server_client_unique1 = std::make_unique<MockClient>();
        // server_client1 = server_client_unique1.get();
        // server_client_shared1 = std::move(server_client_unique1);
        // server_client1->SetType(IClient::Type::kLocalServer);

        local_user1.SetNickname(local_user_nickname1);

        // EXPECT_CALL(server_message1, GetClient())
        //     .WillRepeatedly(Return(server_client_shared1));
        // EXPECT_CALL(server_message1, GetParams())
        //     .WillRepeatedly(ReturnRef(server_message_params1));
        EXPECT_CALL(base_message1, GetClientUUID())
            .WillRepeatedly(Return(base_client1_uuid));
        EXPECT_CALL(base_message1, GetParams())
            .WillRepeatedly(ReturnRef(base_message_params1));

        EXPECT_CALL(local_user_message1, GetClientUUID())
            .WillRepeatedly(Return(local_user1_uuid));
        EXPECT_CALL(local_user_message1, GetParams())
            .WillRepeatedly(ReturnRef(local_user_message_params1));
        
        EXPECT_CALL(base_client1, GetUUID())
            .WillRepeatedly(ReturnRef(base_client1_uuid));
        EXPECT_CALL(local_user1, GetUUID())
            .WillRepeatedly(ReturnRef(local_user1_uuid));

        EXPECT_CALL(mock_client_database, GetClient(local_user1_uuid))
            .WillRepeatedly(Return(std::optional<IClient*>(&local_user1)));
        EXPECT_CALL(mock_client_database, GetClient(base_client1_uuid))
            .WillRepeatedly(Return(std::optional<IClient*>(&base_client1)));
    }
};

TEST_F(QUITTests, RemoveUnregisteredClient)
{
    EXPECT_CALL(mock_client_database, DisconnectClient(base_client1_uuid,
		TypedEq<std::optional<std::string>>(std::nullopt)));
    quit_handler_->Handle(base_message1);
}

TEST_F(QUITTests, RemoveLocalUser)
{
	// Arrange
	std::string quit_message("Quit message");
	local_user_message_params1.push_back(quit_message);

	// Act
    EXPECT_CALL(mock_client_database, DisconnectClient(local_user1_uuid,
		TypedEq<std::optional<std::string>>(std::make_optional<std::string>(quit_message))));
    quit_handler_->Handle(local_user_message1);
}
