#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "NICKHandler.h"
#include "MockClient.h"
#include "MockMessage.h"
#include "MockClientDatabase.h"
#include "MockServerConfig.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class NICKFromUserTests : public ::testing::Test
{
    public:
    MockClient mock_client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();

    MockMessage message1;
    std::vector<std::string> params;

    MockClientDatabase mock_client_database;
    MockServerConfig mock_server_config;

    void SetUp() override
    {
        EXPECT_CALL(mock_client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(params));
        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_client1)));
    }
};

class NICKFromServerTests : public ::testing::Test
{
    public:
    MockClient server_client1;
    IRC::UUID uuid2 = IRC::UUIDGenerator::GetInstance().Generate();
    MockClient user_client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();

    MockMessage message1;
    std::vector<std::string> params;
    MockMessage message2;
    std::vector<std::string> params2;

    MockClientDatabase mock_client_database;
    MockServerConfig mock_server_config;

    void SetUp() override
    {
        server_client1.SetType(IClient::Type::kServer);

        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(params));
        EXPECT_CALL(message2, GetClientUUID())
            .WillRepeatedly(Return(uuid2));
        EXPECT_CALL(message2, GetParams())
            .WillRepeatedly(ReturnRef(params2));
        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&user_client1)));
        EXPECT_CALL(mock_client_database, GetClient(uuid2))
            .WillRepeatedly(Return(std::optional<IClient*>(&server_client1)));

    }
};


TEST_F(NICKFromUserTests, SuccessTest)
{
    params.push_back("new_nickname");

    EXPECT_CALL(mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(mock_client_database, RegisterLocalUser(uuid1))
        .WillOnce(Return(&mock_client1));

    NICKHandler(&mock_server_config, &mock_client_database, message1);

    ASSERT_EQ(mock_client1.GetNickname(), "new_nickname");
}

TEST_F(NICKFromUserTests, NoNickNameGiven)
{
    EXPECT_CALL(mock_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(&mock_server_config, &mock_client_database, message1);
}

TEST_F(NICKFromUserTests, ClientNickCollision)
{
    params.push_back("same_nickname");

    mock_client1.SetNickname("same_nickname");

    EXPECT_CALL(mock_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(&mock_server_config, &mock_client_database, message1);
}

TEST_F(NICKFromServerTests, SuccessTest)
{
    /* Init Message content and set expectations */
    params2.push_back("new_nickname");
    EXPECT_CALL(message2, GetNickname())
        .WillOnce(Return(std::optional<std::string>("old_nickname")));

    /* Set ClientDatabase expectations */
    EXPECT_CALL(mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(mock_client_database, GetClient("old_nickname"))
        .WillOnce(Return(&user_client1));

    /* Init client with old_nickname */
    user_client1.SetNickname("old_nickname");

    NICKHandler(&mock_server_config, &mock_client_database, message2);

    ASSERT_EQ(user_client1.GetNickname(), "new_nickname");
}

TEST_F(NICKFromServerTests, NoOrigin)
{
    params2.push_back("new_nickname");
    /* If GetNickname returns, it means there was no nickname provided as prefix */
    EXPECT_CALL(message2, GetNickname())
        .WillOnce(Return(std::nullopt));

    EXPECT_CALL(server_client1, Push(_)); //TODO add exact invalid msg

    NICKHandler(&mock_server_config, &mock_client_database, message2);
}
