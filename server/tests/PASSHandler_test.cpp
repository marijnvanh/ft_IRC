#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "PASSHandler.h"
#include "IClient.h"
#include "IMessage.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class MockClient : public IClient {
    public:

    MOCK_METHOD1(Push, void(std::string irc_message));
    MOCK_METHOD0(Receive, std::optional<std::string>());
    MOCK_METHOD0(SendAll, void());
    MOCK_CONST_METHOD0(GetUUID, IRC::UUID());
    MOCK_METHOD(IClient::State, GetState, (), (const, override));
    MOCK_METHOD(void, SetState, (IClient::State state), (override));
};

class MockMessage : public IMessage {
    public:

    MOCK_METHOD(const IRC::UUID, GetUUID, (), (const, noexcept));
    MOCK_METHOD(const std::string, GetCommand, (), (const, noexcept));
    MOCK_METHOD(const std::vector<std::string> &, GetParams, (), (const, noexcept));
};

class PASSTests : public ::testing::Test
{
    public:
    std::shared_ptr<IRC::Mutex<IClient>> shared_client1;
    std::unique_ptr<MockClient> unique_client1;
    MockClient *client1;
    MockMessage message1;
    std::vector<std::string> params;

    void SetUp() override
    {
        unique_client1 = std::make_unique<MockClient>();
        client1 = unique_client1.get();
        shared_client1 = std::make_shared<IRC::Mutex<IClient>>(std::move(unique_client1));
    }
};

TEST_F(PASSTests, SuccessTest)
{
    EXPECT_CALL(*client1, GetState())
        .WillOnce(Return(IClient::State::kUnRegistered));
    
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));

    params.push_back("test1");
    PASSHandler(shared_client1, message1);

    ASSERT_EQ(client1->GetPassword(), "test1");
}

TEST_F(PASSTests, InvalidParams)
{
    EXPECT_CALL(*client1, GetState())
        .WillOnce(Return(IClient::State::kUnRegistered));
    
    EXPECT_CALL(message1, GetParams())
        .WillOnce(ReturnRef(params));

    EXPECT_CALL(*client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(shared_client1, message1);
}

TEST_F(PASSTests, AlreadyRegisteredClient)
{
    EXPECT_CALL(*client1, GetState())
        .WillOnce(Return(IClient::State::kRegistered));
    
    EXPECT_CALL(*client1, Push(_)); //TODO add exact invalid msg

    PASSHandler(shared_client1, message1);
}
