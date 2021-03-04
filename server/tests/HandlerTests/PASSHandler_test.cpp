#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "PASSHandler.h"
#include "MockClient.h"
#include "MockClientDatabase.h"
#include "MockMessage.h"
#include "UUID.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class PASSTests : public ::testing::Test
{
    public:
    std::shared_ptr<IClient> shared_client1;
    std::unique_ptr<MockClient> unique_client1;
    MockClient *client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    MockMessage message1;
    std::vector<std::string> params;

    std::shared_ptr<MockClientDatabase> mock_client_database_shared;
    MockClientDatabase *mock_client_database;

    void SetUp() override
    {
        unique_client1 = std::make_unique<MockClient>();
        client1 = unique_client1.get();
        shared_client1 = std::move(unique_client1);

        mock_client_database_shared = std::make_shared<MockClientDatabase>();
        mock_client_database = mock_client_database_shared.get();

        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(params));
        EXPECT_CALL(*mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<std::shared_ptr<IClient>>(shared_client1)));
    }
};

TEST_F(PASSTests, SuccessTest)
{
    params.push_back("test1");
    PASSHandler(mock_client_database_shared, message1);

    ASSERT_EQ(client1->GetPassword(), "test1");
}

TEST_F(PASSTests, InvalidParams)
{
    EXPECT_CALL(*client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(mock_client_database_shared, message1);
}

TEST_F(PASSTests, AlreadyRegisteredClient)
{
    client1->SetState(IClient::State::kRegistered);

    EXPECT_CALL(*client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(mock_client_database_shared, message1);
}
