#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "NICKHandler.h"
#include "MockClient.h"
#include "MockMessage.h"
#include "MockClientDatabase.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class NICKFromUserTests : public ::testing::Test
{
    public:
    std::shared_ptr<IClient> mock_client_shared1;
    std::unique_ptr<MockClient> mock_client_unique1;
    MockClient *mock_client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();

    MockMessage message1;
    std::vector<std::string> message_params;

    std::shared_ptr<MockClientDatabase> mock_client_database_shared;
    MockClientDatabase *mock_client_database;

    void SetUp() override
    {
        mock_client_unique1 = std::make_unique<MockClient>();
        mock_client1 = mock_client_unique1.get();
        mock_client_shared1 = std::move(mock_client_unique1);

        mock_client_database_shared = std::make_shared<MockClientDatabase>();
        mock_client_database = mock_client_database_shared.get();

        EXPECT_CALL(*mock_client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
    }
};

class NICKFromServerTests : public ::testing::Test
{
    public:
    std::shared_ptr<IClient> server_client_shared1;
    std::unique_ptr<MockClient> server_client_unique1;
    MockClient *server_client1;
    std::shared_ptr<IClient> user_client_shared1;
    std::unique_ptr<MockClient> user_client_unique1;
    MockClient *user_client1;

    MockMessage message1;
    std::vector<std::string> message_params;

    std::shared_ptr<MockClientDatabase> mock_client_database_shared;
    MockClientDatabase *mock_client_database;

    void SetUp() override
    {
        server_client_unique1 = std::make_unique<MockClient>();
        server_client1 = server_client_unique1.get();
        server_client_shared1 = std::move(server_client_unique1);
        server_client1->SetType(IClient::Type::kServer);

        user_client_unique1 = std::make_unique<MockClient>();
        user_client1 = user_client_unique1.get();
        user_client_shared1 = std::move(user_client_unique1);

        mock_client_database_shared = std::make_shared<MockClientDatabase>();
        mock_client_database = mock_client_database_shared.get();
    }
};


TEST_F(NICKFromUserTests, SuccessTest)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(mock_client_shared1));
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(message_params));
    message_params.push_back("new_nickname");

    EXPECT_CALL(*mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::nullopt));

    NICKHandler(mock_client_database_shared, message1);

    ASSERT_EQ(mock_client1->GetNickname(), "new_nickname");
}

TEST_F(NICKFromUserTests, NoNickNameGiven)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(mock_client_shared1));
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(message_params));

    EXPECT_CALL(*mock_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(mock_client_database_shared, message1);
}

TEST_F(NICKFromUserTests, ClientNickCollision)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(mock_client_shared1));
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(message_params));
    message_params.push_back("same_nickname");

    mock_client1->SetNickname("same_nickname");

    EXPECT_CALL(*mock_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(mock_client_database_shared, message1);
}

TEST_F(NICKFromServerTests, SuccessTest)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(server_client_shared1));

    /* Init Message content and set expectations */
    message_params.push_back("new_nickname");
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(message_params));
    EXPECT_CALL(message1, GetNickname())
        .WillOnce(Return(std::optional<std::string>("old_nickname")));

    /* Set ClientDatabase expectations */
    EXPECT_CALL(*mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*mock_client_database, GetClient("old_nickname"))
        .WillOnce(Return(user_client_shared1));

    /* Init client with old_nickname */
    user_client1->SetNickname("old_nickname");

    NICKHandler(mock_client_database_shared, message1);

    ASSERT_EQ(user_client1->GetNickname(), "new_nickname");
}

TEST_F(NICKFromServerTests, NoOrigin)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(server_client_shared1));

    message_params.push_back("new_nickname");
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(message_params));
    /* If GetNickname returns, it means there was no nickname provided as prefix */
    EXPECT_CALL(message1, GetNickname())
        .WillOnce(Return(std::nullopt));

    EXPECT_CALL(*server_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(mock_client_database_shared, message1);
}
