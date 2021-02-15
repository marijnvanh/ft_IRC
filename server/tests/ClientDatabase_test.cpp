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


// class MockClient : public IClient {
//     public:
// 
//     MOCK_METHOD1(Push, void(std::shared_ptr<std::string> irc_message));
//     MOCK_METHOD0(Receive, std::optional<std::string>());
//     MOCK_METHOD0(SendAll, void());
//     MOCK_CONST_METHOD0(GetUUID, int());
// };
// 
// /* Clientdatabase and three mock clients */
// class ClientDatabaseTests : public ::testing::Test
// {
//     public:
//     std::unique_ptr<MockClient> unique_client1;
//     MockClient *client1;
//     std::unique_ptr<MockClient> unique_client2;
//     MockClient *client2;
//     std::unique_ptr<MockClient> unique_client3;
//     MockClient *client3;
//     std::shared_ptr<ClientDatabase> client_database;
// 
//     void SetUp() override
//     {
//         unique_client1 = std::make_unique<MockClient>();
//         client1 = unique_client1.get();
//         unique_client2 = std::make_unique<MockClient>();
//         client2 = unique_client2.get();
//         unique_client3 = std::make_unique<MockClient>();
//         client3 = unique_client3.get();
//         client_database = std::make_shared<ClientDatabase>();
//     }
// };
// 
// TEST_F(ClientDatabaseTests, AddAndRemoveClient)
// {
//     EXPECT_CALL(*client1, GetUUID())
//         .WillOnce(Return(1));
//     client_database->AddClient(std::move(unique_client1));
// 
//     client_database->RemoveClient(1);
// }
// 
// TEST_F(ClientDatabaseTests, GetClientByUUID)
// {
//     EXPECT_CALL(*client1, GetUUID())
//         .WillOnce(Return(1));
//     client_database->AddClient(std::move(unique_client1));
// 
//     auto client = client_database->GetClient(1);
//     client->Access([this](IClient &client)
//         {
//             ASSERT_EQ(client1, &client);
//         });
// }
// 
// TEST_F(ClientDatabaseTests, GetInvalidClientByFakeUUID)
// {
//     EXPECT_CALL(*client1, GetUUID())
//         .WillOnce(Return(1));
//     client_database->AddClient(std::move(unique_client1));
// 
//     ASSERT_THROW(client_database->GetClient(2), ClientDatabase::ClientNotFound);
// }
// 
// /* This test adds three clients and expects to receive from all of them.
// This means that the callback should have been called on all three clients */
// TEST_F(ClientDatabaseTests, PollClients)
// {
//     EXPECT_CALL(*client1, GetUUID())
//         .WillOnce(Return(1));
//     EXPECT_CALL(*client2, GetUUID())
//         .WillOnce(Return(2));
//     EXPECT_CALL(*client3, GetUUID())
//         .WillOnce(Return(3));
//     client_database->AddClient(std::move(unique_client1));
//     client_database->AddClient(std::move(unique_client2));
//     client_database->AddClient(std::move(unique_client3));
// 
//     EXPECT_CALL(*client1, Receive())
//         .WillOnce(Return("test"));
//     EXPECT_CALL(*client2, Receive())
//         .WillOnce(Return("test1"));
//     EXPECT_CALL(*client3, Receive())
//         .WillOnce(Return("test2"));
// 
//     int callback_count = 0;
//     client_database->PollClients([&callback_count](int uuid, std::string message) {
//         (void)message;
//         (void)uuid;
//         callback_count += 1;
//     });
// 
//     EXPECT_EQ(callback_count, 3);
// }
// 
// /* Test if a disconnected client gets removed from the list after polling */
// TEST_F(ClientDatabaseTests, PollDisconnectedClient)
// {
//     EXPECT_CALL(*client1, GetUUID())
//         .WillOnce(Return(1));
//     client_database->AddClient(std::move(unique_client1));
// 
//     EXPECT_CALL(*client1, Receive())
//         .WillOnce(Throw(IClient::Disconnected("test")));
// 
//     client_database->PollClients([](int uuid, std::string message) {
//         (void)message;
//         (void)uuid;
//     });
// 
//     ASSERT_THROW(client_database->GetClient(1), ClientDatabase::ClientNotFound);
// }
// 
// /* This test adds three clients and expects to call SendAll from all of them. */
// TEST_F(ClientDatabaseTests, SendAllWithThreeClients)
// {
//     EXPECT_CALL(*client1, GetUUID())
//         .WillOnce(Return(1));
//     EXPECT_CALL(*client2, GetUUID())
//         .WillOnce(Return(2));
//     EXPECT_CALL(*client3, GetUUID())
//         .WillOnce(Return(3));
//     client_database->AddClient(std::move(unique_client1));
//     client_database->AddClient(std::move(unique_client2));
//     client_database->AddClient(std::move(unique_client3));
// 
//     EXPECT_CALL(*client1, SendAll());
//     EXPECT_CALL(*client2, SendAll());
//     EXPECT_CALL(*client3, SendAll());
// 
//     client_database->SendAll();
// }
// 
// /* Test if a disconnected client gets removed from the list after polling */
// TEST_F(ClientDatabaseTests, SendAllWithDisconnectedClient)
// {
//     EXPECT_CALL(*client1, GetUUID())
//         .WillOnce(Return(1));
//     client_database->AddClient(std::move(unique_client1));
// 
//     EXPECT_CALL(*client1, SendAll())
//         .WillOnce(Throw(IClient::Disconnected("test")));
// 
//     client_database->SendAll();
// 
//     ASSERT_THROW(client_database->GetClient(1), ClientDatabase::ClientNotFound);
// }
