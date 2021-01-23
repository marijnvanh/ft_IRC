#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IClient.h"
#include "ClientDatabase.h"
#include <memory>
#include <optional>
#include "Mutex.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::_;


class MockClient : public IClient {
    public:

    MOCK_METHOD1(Push, void(std::shared_ptr<std::string> irc_message));
    MOCK_METHOD0(Receive, std::optional<std::string>());
    MOCK_METHOD0(SendAll, void());
    MOCK_CONST_METHOD0(GetUUID, int());
};

class ClientDatabaseTests : public ::testing::Test
{
    public:
    std::unique_ptr<MockClient> unique_client1;
    MockClient *client1;
    std::unique_ptr<MockClient> unique_client2;
    MockClient *client2;
    std::unique_ptr<MockClient> unique_client3;
    MockClient *client3;
    std::shared_ptr<ClientDatabase> client_database;

    void SetUp() override
    {
        unique_client1 = std::make_unique<MockClient>();
        client1 = unique_client1.get();
        unique_client2 = std::make_unique<MockClient>();
        client2 = unique_client2.get();
        unique_client3 = std::make_unique<MockClient>();
        client3 = unique_client3.get();
        client_database = std::make_shared<ClientDatabase>();
    }
};

//TODO assert properly
TEST_F(ClientDatabaseTests, AddAndDeleteClient)
{
    EXPECT_CALL(*client1, GetUUID())
          .WillOnce(Return(1));
    EXPECT_CALL(*client2, GetUUID())
          .WillOnce(Return(2));
    EXPECT_CALL(*client3, GetUUID())
          .WillOnce(Return(3));
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    client_database->RemoveClient(1);
    client_database->RemoveClient(2);
    client_database->RemoveClient(3);
}

TEST_F(ClientDatabaseTests, GetClientByUUID)
{
    EXPECT_CALL(*client1, GetUUID())
          .WillOnce(Return(1));
    client_database->AddClient(std::move(unique_client1));

    ASSERT_EQ((*client_database)[1], 1);
}

TEST_F(ClientDatabaseTests, GetInvalidClientByFakeUUID)
{
    EXPECT_CALL(*client1, GetUUID())
          .WillOnce(Return(1));
    client_database->AddClient(std::move(unique_client1));

    ASSERT_THROW((*client_database)[2], ClientDatabase::ClientNotFound);
}

TEST_F(ClientDatabaseTests, PollClients)
{
    EXPECT_CALL(*client1, GetUUID())
          .WillOnce(Return(1));
    EXPECT_CALL(*client2, GetUUID())
          .WillOnce(Return(2));
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));

    EXPECT_CALL(*client1, Receive())
          .WillOnce(Return("test"));
    EXPECT_CALL(*client2, Receive())
          .WillOnce(Return("test1"));

    int callback_count = 0;
    client_database->PollClients([&](std::string message) {
        (void)message;
        callback_count += 1;
    });

    EXPECT_EQ(callback_count, 2);
}