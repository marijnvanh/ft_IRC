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
    std::shared_ptr<IRC::Mutex<IClient>> mock_client_shared1;
    std::unique_ptr<MockClient> mock_client_unique1;
    MockClient *mock_client1;
    std::shared_ptr<IRC::Mutex<IClient>> mock_client_shared2;
    std::unique_ptr<MockClient> mock_client_unique2;
    MockClient *mock_client2;

    MockMessage message1;
    std::vector<std::string> params;

    std::shared_ptr<MockClientDatabase> mock_client_database_shared;
    MockClientDatabase *mock_client_database;

    void SetUp() override
    {
        mock_client_unique1 = std::make_unique<MockClient>();
        mock_client1 = mock_client_unique1.get();
        mock_client_shared1 = std::make_shared<IRC::Mutex<IClient>>(std::move(mock_client_unique1));
        mock_client_unique2 = std::make_unique<MockClient>();
        mock_client2 = mock_client_unique2.get();
        mock_client_shared2 = std::make_shared<IRC::Mutex<IClient>>(std::move(mock_client_unique2));


        mock_client_database_shared = std::make_shared<MockClientDatabase>();
        mock_client_database = mock_client_database_shared.get();
    }
};

TEST_F(NICKTests, NICKFromUserSuccessTest)
{
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));
    params.push_back("new_nickname");

    EXPECT_CALL(*mock_client_database, Find(_))
        .WillOnce(Return(std::nullopt));

    NICKHandler(mock_client_database_shared, mock_client_shared1, message1);

    ASSERT_EQ(mock_client1->GetNickname(), "new_nickname");
}

TEST_F(NICKTests, NoNickNameGiven)
{
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));

    EXPECT_CALL(*mock_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(mock_client_database_shared, mock_client_shared1, message1);
}

TEST_F(NICKTests, ClientNickCollision)
{
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));
    params.push_back("same_nickname");

    mock_client1->SetNickname("same_nickname");

    EXPECT_CALL(*mock_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(mock_client_database_shared, mock_client_shared1, message1);
}

//TODO Fix this hacky mess with it's own fixture
TEST_F(NICKTests, NICKFromServerSuccessTest)
{
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));
    params.push_back("new_nickname");

    EXPECT_CALL(*mock_client_database, Find(_))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(*mock_client_database, Find("old_nickname"))
        .WillOnce(Return(mock_client_shared2));

    EXPECT_CALL(message1, GetNickname())
        .WillOnce(Return(std::optional<std::string>("old_nickname")));
    mock_client1->SetType(IClient::Type::kServer);
    mock_client2->SetNickname("old_nickname");

    NICKHandler(mock_client_database_shared, mock_client_shared1, message1);

    ASSERT_EQ(mock_client2->GetNickname(), "new_nickname");
}