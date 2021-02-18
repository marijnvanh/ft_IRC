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
using ::testing::ReturnRef;
using ::testing::_;


class MockClient : public IClient {
    public:

    MOCK_METHOD1(Push, void(std::string irc_message));
    MOCK_METHOD0(Receive, std::optional<std::string>());
    MOCK_METHOD0(SendAll, void());
    MOCK_CONST_METHOD0(GetUUID, const IRC::UUID&());
};

/* Clientdatabase and three mock clients */
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

    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    IRC::UUID uuid2 = IRC::UUIDGenerator::GetInstance().Generate();
    IRC::UUID uuid3 = IRC::UUIDGenerator::GetInstance().Generate();

    void SetUp() override
    {
        unique_client1 = std::make_unique<MockClient>();
        client1 = unique_client1.get();
        unique_client2 = std::make_unique<MockClient>();
        client2 = unique_client2.get();
        unique_client3 = std::make_unique<MockClient>();
        client3 = unique_client3.get();
        client_database = std::make_shared<ClientDatabase>();

        EXPECT_CALL(*client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        EXPECT_CALL(*client2, GetUUID())
            .WillRepeatedly(ReturnRef(uuid2));
        EXPECT_CALL(*client3, GetUUID())
            .WillRepeatedly(ReturnRef(uuid3));
    }
};

TEST_F(ClientDatabaseTests, AddAndRemoveClient)
{
    client_database->AddClient(std::move(unique_client1));

    ASSERT_EQ(client_database->GetClient(uuid1)->Take()->GetUUID(), uuid1);

    client_database->RemoveClient(uuid1);

    ASSERT_THROW(client_database->GetClient(uuid1), ClientDatabase::ClientNotFound);
}

TEST_F(ClientDatabaseTests, GetInvalidClientByFakeUUID)
{
    client_database->AddClient(std::move(unique_client1));

    ASSERT_THROW(client_database->GetClient(uuid2), ClientDatabase::ClientNotFound);
}

/* This test adds three clients and expects to receive from all of them.
   This means that the callback should have been called on all three clients */
TEST_F(ClientDatabaseTests, PollClients)
{
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    EXPECT_CALL(*client1, Receive())
        .WillOnce(Return("test"));
    EXPECT_CALL(*client2, Receive())
        .WillOnce(Return("test1"));
    EXPECT_CALL(*client3, Receive())
        .WillOnce(Return("test2"));


    std::unordered_map<IRC::UUID, std::string> expectations = {
        { uuid1, "test" },
        { uuid2, "test1" },
        { uuid3, "test2" }
    };

    int callback_count = 0;
    client_database->PollClients([&callback_count, &expectations](IRC::UUID uuid, std::string message) {
        EXPECT_EQ(expectations[uuid], message);
        callback_count += 1;
    });

    EXPECT_EQ(callback_count, 3);
}
 
/* Test if a disconnected client gets removed from the list after polling */
TEST_F(ClientDatabaseTests, PollDisconnectedClient)
{
    client_database->AddClient(std::move(unique_client1));

    EXPECT_CALL(*client1, Receive())
        .WillOnce(Throw(IClient::Disconnected("test")));

    client_database->PollClients([](IRC::UUID uuid, std::string message) {
        (void)message;
        (void)uuid;
    });

    ASSERT_THROW(client_database->GetClient(uuid1), ClientDatabase::ClientNotFound);
}

/* This test adds three clients and expects to call SendAll from all of them. */
TEST_F(ClientDatabaseTests, SendAllWithThreeClients)
{
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    EXPECT_CALL(*client1, SendAll());
    EXPECT_CALL(*client2, SendAll());
    EXPECT_CALL(*client3, SendAll());
   
    client_database->SendAll();
}

/* Test if a disconnected client gets removed from the list after polling */
TEST_F(ClientDatabaseTests, SendAllWithDisconnectedClient)
{
    client_database->AddClient(std::move(unique_client1));

    EXPECT_CALL(*client1, SendAll())
        .WillOnce(Throw(IClient::Disconnected("test")));

    client_database->SendAll();

    ASSERT_THROW(client_database->GetClient(uuid1), ClientDatabase::ClientNotFound);
}

TEST_F(ClientDatabaseTests, FindNickname)
{
    unique_client1->SetNickname("test1");
    unique_client2->SetNickname("test2");
    unique_client3->SetNickname("test3");
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    std::string nickname_to_find("test2");
    auto client = client_database->Find(nickname_to_find);
    ASSERT_EQ((*client)->Take()->GetNickname(), nickname_to_find);
}

TEST_F(ClientDatabaseTests, FindNicknameDoesNotExist)
{
    unique_client1->SetNickname("test1");
    unique_client2->SetNickname("test2");
    unique_client3->SetNickname("test3");
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    std::string nickname_to_find("none");
    auto client = client_database->Find(nickname_to_find);
    ASSERT_EQ(client, std::nullopt);
}
