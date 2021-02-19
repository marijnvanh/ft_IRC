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

class NICKTests : public ::testing::Test
{
    public:
    std::shared_ptr<IRC::Mutex<IClient>> mock_client_shared;
    std::unique_ptr<MockClient> mock_client_unique;
    MockClient *mock_client;

    MockMessage message1;
    std::vector<std::string> params;

    std::shared_ptr<MockClientDatabase> mock_client_database_shared;
    MockClientDatabase *mock_client_database;

    void SetUp() override
    {
        mock_client_unique = std::make_unique<MockClient>();
        mock_client = mock_client_unique.get();
        mock_client_shared = std::make_shared<IRC::Mutex<IClient>>(std::move(mock_client_unique));


        mock_client_database_shared = std::make_shared<MockClientDatabase>();
        mock_client_database = mock_client_database_shared.get();
    }
};

TEST_F(NICKTests, SuccessTest)
{
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));
    params.push_back("new_nickname");

    EXPECT_CALL(*mock_client_database, Find(_))
        .WillOnce(Return(std::nullopt));

    NICKHandler(mock_client_database_shared, mock_client_shared, message1);

    ASSERT_EQ(mock_client->GetNickname(), "new_nickname");
}

TEST_F(NICKTests, NoNickNameGiven)
{
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));

    EXPECT_CALL(*mock_client, Push(_)); //TODO add exact invalid msg

    NICKHandler(mock_client_database_shared, mock_client_shared, message1);
}

TEST_F(NICKTests, ClientNickCollision)
{
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));
    params.push_back("same_nickname");

    mock_client->SetNickname("same_nickname");

    EXPECT_CALL(*mock_client, Push(_)); //TODO add exact invalid msg

    NICKHandler(mock_client_database_shared, mock_client_shared, message1);
}
