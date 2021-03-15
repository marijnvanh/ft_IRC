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
    MockClient mock_client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    MockMessage message1;
    std::vector<std::string> params;

    MockClientDatabase mock_client_database;

    void SetUp() override
    {
        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(params));
        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_client1)));
    }
};

TEST_F(PASSTests, SuccessTest)
{
    params.push_back("test1");
    PASSHandler(&mock_client_database, message1);

    ASSERT_EQ(mock_client1.GetPassword(), "test1");
}

TEST_F(PASSTests, InvalidParams)
{
    EXPECT_CALL(mock_client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(&mock_client_database, message1);
}

TEST_F(PASSTests, AlreadyRegisteredClient)
{
    mock_client1.SetState(IClient::State::kRegistered);

    EXPECT_CALL(mock_client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(&mock_client_database, message1);
}