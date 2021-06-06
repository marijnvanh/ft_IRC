#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "NICKHandler.h"
#include "MockClient.h"
#include "MockUser.h"
#include "MockServer.h"
#include "MockMessage.h"
#include "MockClientDatabase.h"
#include "MockServerConfig.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::StrictMock;

class NICKHandlerTests : public ::testing::Test
{
    public:

	std::unique_ptr<NICKHandler> nick_handler_;
    StrictMock<MockClient> mock_client1;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    StrictMock<MockServer> server_client1;
    IRC::UUID uuid2 = IRC::UUIDGenerator::GetInstance().Generate();
    StrictMock<MockUser> user_client1;
    IRC::UUID uuid3 = IRC::UUIDGenerator::GetInstance().Generate();

    StrictMock<MockMessage> client_message1;
    std::vector<std::string> client_params;
    StrictMock<MockMessage> server_message1;
    std::vector<std::string> server_params;

	std::string server1_name;

    StrictMock<MockClientDatabase> mock_client_database;
    MockServerConfig mock_server_config;
    MockServer mock_server;

    void SetUp() override
    {
		nick_handler_ = std::make_unique<NICKHandler>(&mock_server_config, &mock_client_database);
        server_client1.SetType(IClient::Type::kLocalServer);
        user_client1.SetType(IClient::Type::kLocalUser);

		server1_name = std::string("test.test.tst");

        EXPECT_CALL(mock_client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_client1)));

        EXPECT_CALL(server_client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid2));
		EXPECT_CALL(server_client1, GetServerName())
			.WillRepeatedly(ReturnRef(server1_name));
        EXPECT_CALL(mock_client_database, GetClient(uuid2))
            .WillRepeatedly(Return(std::optional<IClient*>(&server_client1)));

        EXPECT_CALL(user_client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid3));
        EXPECT_CALL(mock_client_database, GetClient(uuid3))
            .WillRepeatedly(Return(std::optional<IClient*>(&user_client1)));

        EXPECT_CALL(client_message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(client_message1, GetParams())
            .WillRepeatedly(ReturnRef(client_params));

        EXPECT_CALL(server_message1, GetClientUUID())
            .WillRepeatedly(Return(uuid2));
        EXPECT_CALL(server_message1, GetParams())
            .WillRepeatedly(ReturnRef(server_params));
    }
};


TEST_F(NICKHandlerTests, SimpleNicknameFromUnregisteredClient)
{
    client_params.push_back("new_nickname");

    EXPECT_CALL(mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(mock_client_database, RegisterLocalUser(uuid1))
        .WillOnce(Return(&mock_client1));
    EXPECT_CALL(mock_client1, Push(_))
        .Times(1);

    nick_handler_->Handle(client_message1);

    ASSERT_EQ(mock_client1.GetNickname(), "new_nickname");
}

TEST_F(NICKHandlerTests, NoNickNameGiven)
{
    EXPECT_CALL(mock_client1, Push(": 461 NICK :Not enough parameters"));

    nick_handler_->Handle(client_message1);
}

TEST_F(NICKHandlerTests, ClientNickCollision)
{
    client_params.push_back("same_nickname");
    mock_client1.SetNickname("same_nickname");

    EXPECT_CALL(mock_client1, Push(": 436 :Nickname collision KILL"));

    nick_handler_->Handle(client_message1);
}

TEST_F(NICKHandlerTests, NicknameChangeFromRemoteUser)
{
    /* Init Message content and set expectations */
    server_params.push_back("new_nickname");
    EXPECT_CALL(server_message1, GetNickname())
        .WillOnce(Return(std::optional<std::string>("old_nickname")));

    /* Set ClientDatabase expectations */
    EXPECT_CALL(mock_client_database, GetUser("old_nickname"))
        .WillOnce(Return(std::optional<IUser*>(&user_client1)));
    EXPECT_CALL(mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::nullopt));
	EXPECT_CALL(user_client1, GetRemoteServer())
		.WillOnce(Return(&server_client1));

    /* Init client with old_nickname */
    user_client1.SetNickname("old_nickname");

    EXPECT_CALL(mock_client_database, BroadcastToLocalServers(_,_))
        .Times(1);

    nick_handler_->Handle(server_message1);

    ASSERT_EQ(user_client1.GetNickname(), "new_nickname");
}

TEST_F(NICKHandlerTests, NoOrigin)
{
    server_params.push_back("new_nickname");
    server_params.push_back("fake_hop_count");
    /* If GetNickname returns, it means there was no nickname provided as prefix */
    EXPECT_CALL(server_message1, GetNickname())
        .WillOnce(Return(std::nullopt));

    EXPECT_CALL(server_client1, Push(": 431 :No nickname given"));

    nick_handler_->Handle(server_message1);
}

TEST_F(NICKHandlerTests, RemoteUserRegistration)
{
    std::string servername = "servername";
    server_params.push_back("new_nickname");
    server_params.push_back("fake_hop_count");
    server_params.push_back("new_username");
    server_params.push_back(servername);
    server_params.push_back("1");
    server_params.push_back("+i");
    server_params.push_back(":realname");

    /* Set ClientDatabase expectations */
    EXPECT_CALL(mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(mock_client_database, GetServer(1))
        .WillOnce(Return(std::optional<IServer*>(&mock_server)));
    EXPECT_CALL(mock_server, GetServerName())
        .WillRepeatedly(ReturnRef(servername));
    EXPECT_CALL(mock_client_database, AddRemoteUser(_))
        .Times(1);
    EXPECT_CALL(mock_client_database, BroadcastToLocalServers(_,_))
        .Times(1);
    EXPECT_CALL(mock_server, AddClient(_))
        .Times(1);
    EXPECT_CALL(mock_server, RemoveClient(_))
        .Times(1);

    nick_handler_->Handle(server_message1);
}

TEST_F(NICKHandlerTests, UserRegistrationCollision)
{
    server_params.push_back("new_nickname");
    server_params.push_back("fake_hop_count");
    server_params.push_back("new_username");
    server_params.push_back("servername");
    server_params.push_back("randomtoken");
    server_params.push_back("umode");
    server_params.push_back("realname");

    /* Set ClientDatabase expectations */
    EXPECT_CALL(mock_client_database, GetClient("new_nickname"))
        .WillOnce(Return(std::optional<IClient*>(&user_client1)));
    EXPECT_CALL(mock_client_database, DisconnectClient(uuid3, _));

    nick_handler_->Handle(server_message1);
}
