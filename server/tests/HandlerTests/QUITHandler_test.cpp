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
using ::testing::_;

class QUITTests : public ::testing::Test
{
    public:
    // std::shared_ptr<IClient> server_client_shared1;
    // std::unique_ptr<MockClient> server_client_unique1;
    // MockClient *server_client1;
    // MockMessage server_message1;
    // std::vector<std::string> server_message_params1;

    std::shared_ptr<MockLocalUser> local_user_shared1;
    MockLocalUser *local_user1;
    MockMessage local_user_message1;
    std::string local_user_nickname1;
    std::vector<std::string> local_user_message_params1;

    std::shared_ptr<IClient> base_client_shared1;
    std::unique_ptr<MockClient> base_client_unique1;
    MockClient *base_client1;
    MockMessage base_message1;
    std::vector<std::string> base_message_params1;
    IRC::UUID base_client1_uuid = IRC::UUIDGenerator::GetInstance().Generate();

    std::shared_ptr<MockClientDatabase> mock_client_database_shared;
    MockClientDatabase *mock_client_database;

    void SetUp() override
    {
        // server_client_unique1 = std::make_unique<MockClient>();
        // server_client1 = server_client_unique1.get();
        // server_client_shared1 = std::move(server_client_unique1);
        // server_client1->SetType(IClient::Type::kServer);

        local_user_shared1 = std::make_shared<MockLocalUser>();
        local_user1 = local_user_shared1.get();
        local_user_shared1->SetNickname(local_user_nickname1);

        base_client_unique1 = std::make_unique<MockClient>();
        base_client1 = base_client_unique1.get();
        base_client_shared1 = std::move(base_client_unique1);

        mock_client_database_shared = std::make_shared<MockClientDatabase>();
        mock_client_database = mock_client_database_shared.get();

        // EXPECT_CALL(server_message1, GetClient())
        //     .WillRepeatedly(Return(server_client_shared1));
        // EXPECT_CALL(server_message1, GetParams())
        //     .WillRepeatedly(ReturnRef(server_message_params1));
        EXPECT_CALL(base_message1, GetClient())
            .WillRepeatedly(Return(base_client_shared1));
        EXPECT_CALL(base_message1, GetParams())
            .WillRepeatedly(ReturnRef(base_message_params1));
        EXPECT_CALL(local_user_message1, GetClient())
            .WillRepeatedly(Return(local_user_shared1));
        EXPECT_CALL(local_user_message1, GetParams())
            .WillRepeatedly(ReturnRef(local_user_message_params1));
        
        EXPECT_CALL(*base_client1, GetUUID())
            .WillRepeatedly(ReturnRef(base_client1_uuid));
    }
};

TEST_F(QUITTests, RemoveUnregisteredClient)
{
    EXPECT_CALL(*mock_client_database, RemoveClient(base_client1_uuid));
    QUITHandler(mock_client_database_shared, base_message1);
}

TEST_F(QUITTests, RemoveLocalUser)
{
    EXPECT_CALL(*mock_client_database, RemoveUser(local_user_nickname1));
    QUITHandler(mock_client_database_shared, local_user_message1);
}
