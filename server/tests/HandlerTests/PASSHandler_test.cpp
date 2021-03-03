#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "PASSHandler.h"
#include "MockClient.h"
#include "MockMessage.h"

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
    MockMessage message1;
    std::vector<std::string> params;

    void SetUp() override
    {
        unique_client1 = std::make_unique<MockClient>();
        client1 = unique_client1.get();
        shared_client1 = std::move(unique_client1);
    }
};

TEST_F(PASSTests, SuccessTest)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(shared_client1));

    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));

    params.push_back("test1");
    PASSHandler(message1);

    ASSERT_EQ(client1->GetPassword(), "test1");
}

TEST_F(PASSTests, InvalidParams)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(shared_client1));

    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));

    EXPECT_CALL(*client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(message1);
}

TEST_F(PASSTests, AlreadyRegisteredClient)
{
    EXPECT_CALL(message1, GetClient())
        .WillOnce(Return(shared_client1));

    client1->SetState(IClient::State::kRegistered);

    EXPECT_CALL(*client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(message1);
}
