#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IClient.h"
#include "ClientDatabase.h"
#include <memory>
#include <optional>
#include "Mutex.h"
#include "MockClient.h"
#include "MockLocalUser.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;


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

    std::unique_ptr<MockLocalUser> unique_local_user1;
    MockLocalUser *local_user1;


    std::shared_ptr<ClientDatabase> client_database;

    IRC::UUID uuid1 = IRC::UUIDGenerator::GetInstance().Generate();
    IRC::UUID uuid2 = IRC::UUIDGenerator::GetInstance().Generate();
    IRC::UUID uuid3 = IRC::UUIDGenerator::GetInstance().Generate();
    IRC::UUID uuid4 = IRC::UUIDGenerator::GetInstance().Generate();

    void SetUp() override
    {
        unique_client1 = std::make_unique<MockClient>();
        client1 = unique_client1.get();
        unique_client2 = std::make_unique<MockClient>();
        client2 = unique_client2.get();
        unique_client3 = std::make_unique<MockClient>();
        client3 = unique_client3.get();

        unique_local_user1 = std::make_unique<MockLocalUser>();
        local_user1 = unique_local_user1.get();
        
        client_database = std::make_shared<ClientDatabase>();

        EXPECT_CALL(*client1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid1));
        EXPECT_CALL(*client2, GetUUID())
            .WillRepeatedly(ReturnRef(uuid2));
        EXPECT_CALL(*client3, GetUUID())
            .WillRepeatedly(ReturnRef(uuid3));
        EXPECT_CALL(*local_user1, GetUUID())
            .WillRepeatedly(ReturnRef(uuid4));
    }
};

TEST_F(ClientDatabaseTests, AddAndRemoveClient)
{
    client_database->AddClient(std::move(unique_client1));

    ASSERT_EQ((*client_database->GetClient(uuid1))->GetUUID(), uuid1);

    client_database->RemoveClient(uuid1);

    ASSERT_EQ(client_database->GetClient(uuid1), std::nullopt);
}

TEST_F(ClientDatabaseTests, GetInvalidClientByFakeUUID)
{
    client_database->AddClient(std::move(unique_client1));

    ASSERT_EQ(client_database->GetClient(uuid2), std::nullopt);
}

/* This test adds three clients and expects to receive from all of them.
   This means that the callback should have been called on all three clients */
TEST_F(ClientDatabaseTests, PollClients)
{
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    EXPECT_CALL(*client1, Receive())
        .WillOnce(Return("test"))
        .WillRepeatedly(Return(std::nullopt));
    EXPECT_CALL(*client2, Receive())
        .WillOnce(Return("test1"))
        .WillRepeatedly(Return(std::nullopt));
    EXPECT_CALL(*client3, Receive())
        .WillOnce(Return("test2"))
        .WillRepeatedly(Return(std::nullopt));


    std::unordered_map<IRC::UUID, std::string> expectations = {
        { uuid1, "test" },
        { uuid2, "test1" },
        { uuid3, "test2" }
    };

    int callback_count = 0;
    client_database->PollClients([&callback_count, &expectations](IClient* client, std::string message) {
        EXPECT_EQ(expectations[client->GetUUID()], message);
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

    client_database->PollClients([](IClient* client, std::string message) {
        (void)client;
        (void)message;
    });

    ASSERT_EQ(client_database->GetClient(uuid1), std::nullopt);
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

    ASSERT_EQ(client_database->GetClient(uuid1), std::nullopt);
}

TEST_F(ClientDatabaseTests, GetClientByNickname)
{
    unique_client1->SetNickname("test1");
    unique_client2->SetNickname("test2");
    unique_client3->SetNickname("test3");
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    std::string nickname_to_find("test2");
    auto client = client_database->GetClient(nickname_to_find);
    ASSERT_EQ((*client)->GetNickname(), nickname_to_find);
}

TEST_F(ClientDatabaseTests, NicknameDoesNotExist)
{
    unique_client1->SetNickname("test1");
    unique_client2->SetNickname("test2");
    unique_client3->SetNickname("test3");
    client_database->AddClient(std::move(unique_client1));
    client_database->AddClient(std::move(unique_client2));
    client_database->AddClient(std::move(unique_client3));

    std::string nickname_to_find("none");
    auto client = client_database->GetClient(nickname_to_find);
    ASSERT_EQ(client, std::nullopt);
}

/* GetClient by nickname should return both Registered and UnRegistered client */
TEST_F(ClientDatabaseTests, GetRegisteredAndUnregisteredClient)
{
    std::string client_nickname("nickname1");
    std::string localuser_nickname("nickname2");
    
    unique_client1->SetNickname(client_nickname);
    local_user1->SetNickname(localuser_nickname);
    client_database->AddClient(std::move(unique_client1));
    client_database->AddLocalUser(std::move(unique_local_user1));

    auto client = client_database->GetClient(client_nickname);
    ASSERT_EQ((*client)->GetNickname(), client_nickname);
    
    auto local_user = client_database->GetClient(localuser_nickname);
    ASSERT_EQ((*local_user)->GetNickname(), localuser_nickname);
}
