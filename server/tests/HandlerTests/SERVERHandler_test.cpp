#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "SERVERHandler.h"
#include "MockMessage.h"
#include "MockClient.h"
#include "MockServer.h"
#include "MockServerConfig.h"
#include "MockClientDatabase.h"
#include "MockChannelDatabase.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;
using ::testing::StrictMock;

class SERVERHandlerTests : public ::testing::Test
{
    public:
	std::unique_ptr<SERVERHandler> server_handler_;
    MockServerConfig mock_server_config_;

	StrictMock<MockClient> mock_client1;

	std::string new_server_name1;
	ServerConnectData new_server_connect_data;
    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    
    MockMessage message1;
    std::vector<std::string> message_params;

    StrictMock<MockClientDatabase> mock_client_database;
    StrictMock<MockChannelDatabase> mock_channel_database;

    MockServer mock_server1;
    IRC::UUID uuid2 = IRC::UUIDGenerator::GetInstance().Generate();

    MockServer mock_remote_server;
    std::string remote_server_name = "SomeOtherServer";

    void SetUp() override
    {
		server_handler_ = std::make_unique<SERVERHandler>(&mock_server_config_, &mock_client_database, &mock_channel_database);

		new_server_connect_data.SetName(new_server_name1);
		EXPECT_CALL(mock_server_config_, GetAuthorizedServer(new_server_name1))
			.WillRepeatedly(Return(std::optional<ServerConnectData*>(&new_server_connect_data)));

        EXPECT_CALL(mock_client_database, GetClient(uuid1))
            .WillRepeatedly(Return(std::optional<IClient*>(&mock_client1)));

        EXPECT_CALL(mock_client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        EXPECT_CALL(message1, GetClientUUID())
            .WillRepeatedly(Return(uuid1));
        EXPECT_CALL(message1, GetParams())
            .WillRepeatedly(ReturnRef(message_params));
        EXPECT_CALL(mock_server1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid2));
    }

	void TearDown() override
	{}
};

TEST_F(SERVERHandlerTests, RegularUserTryingToSendSERVERMessage)
{
    message_params.push_back(new_server_name1);
    message_params.push_back("ignored");
    message_params.push_back("ignored");

    mock_client1.SetType(IClient::Type::kLocalUser);

    EXPECT_CALL(mock_client1, Push(": 462 :You may not reregister"));

    server_handler_->Handle(message1);
}

TEST_F(SERVERHandlerTests, DuplicateServersOnNetwork)
{
    message_params.push_back(new_server_name1);
    message_params.push_back("ignored");
    message_params.push_back("ignored");

    mock_client1.SetType(IClient::Type::kLocalUser);
    mock_client1.SetType(IClient::Type::kLocalServer);
    
    EXPECT_CALL(mock_client_database, GetServer(new_server_name1))
        .WillOnce(Return(std::optional<IServer*>(&mock_server1)));
	EXPECT_CALL(mock_client_database, DisconnectClient(uuid1, _));

    server_handler_->Handle(message1);
}

TEST_F(SERVERHandlerTests, ServerRegisteringRemoteServer)
{
    message_params.push_back(new_server_name1);
    message_params.push_back("ignored");
    message_params.push_back("ignored");

    mock_client1.SetType(IClient::Type::kLocalUser);
    mock_client1.SetType(IClient::Type::kLocalServer);
    
    EXPECT_CALL(mock_client_database, GetServer(new_server_name1))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(message1, GetServername())
        .WillOnce(Return(std::optional<std::string>(remote_server_name)));
    EXPECT_CALL(mock_client_database, GetServer(remote_server_name))
        .WillOnce(Return(std::optional<IServer*>(&mock_remote_server)));
    EXPECT_CALL(mock_client_database, AddServer(_));
    EXPECT_CALL(mock_remote_server, AddClient(_));
    EXPECT_CALL(mock_remote_server, RemoveClient(_));
    EXPECT_CALL(mock_client_database, BroadcastToLocalServers(_, std::make_optional<IRC::UUID>(uuid1)));

    server_handler_->Handle(message1);
}

TEST_F(SERVERHandlerTests, ClientRegisteringAsServer)
{
    message_params.push_back(new_server_name1);
    message_params.push_back("ignored");
    message_params.push_back("ignored");

    mock_client1.SetType(IClient::Type::kUnRegistered);
    
    EXPECT_CALL(mock_client_database, GetServer(new_server_name1))
        .WillOnce(Return(std::nullopt));
    EXPECT_CALL(mock_client_database, RegisterLocalServer(new_server_name1, uuid1))
        .WillOnce(Return(&mock_server1));
    EXPECT_CALL(mock_server1, Push(_))
		.Times(2);
    EXPECT_CALL(mock_client_database, BroadcastToLocalServers(_, std::make_optional<IRC::UUID>(uuid1)));
    EXPECT_CALL(mock_client_database, DoForEachServer(_, _));
    EXPECT_CALL(mock_client_database, DoForEachUser(_, _));
    EXPECT_CALL(mock_channel_database, ForEachChannel(_));

    server_handler_->Handle(message1);
}
