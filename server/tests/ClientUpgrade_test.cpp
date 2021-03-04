#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <memory>
#include <optional>

#include "ClientDatabase.h"
#include "MockIOHandler.h"
#include "MockLocalUser.h"
#include "Client.h"
#include "MockServer.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class ClientUpgradeTests : public ::testing::Test
{
    public:
    std::shared_ptr<MockServer> mock_server;

    std::unique_ptr<MockIOHandler> io_handler1;
    std::unique_ptr<Client> client1;
    Client *raw_client1;
    std::unique_ptr<MockIOHandler> io_handler2;
    std::unique_ptr<Client> client2;
    Client *raw_client2;

    std::shared_ptr<ClientDatabase> client_database;

    void SetUp() override
    {
        mock_server = std::make_shared<MockServer>();

        io_handler1 = std::make_unique<MockIOHandler>();
        client1 = std::make_unique<Client>(std::move(io_handler1), mock_server);
        raw_client1= client1.get();
        io_handler2 = std::make_unique<MockIOHandler>();
        client2 = std::make_unique<Client>(std::move(io_handler2), mock_server);
        raw_client2 = client2.get();

        client_database = std::make_shared<ClientDatabase>();

        client_database->AddClient(std::move(client1));
        client_database->AddClient(std::move(client2));
        
        raw_client1->SetNickname("test");
        raw_client1->SetUsername("test");
        raw_client2->SetNickname("test");
        raw_client2->SetUsername("test");
    }
};

TEST_F(ClientUpgradeTests, RegisterLocalUser)
{
    auto uuid_client1 = raw_client1->GetUUID();

    auto client = client_database->GetClient(uuid_client1);
    ASSERT_EQ((*client)->GetType(), IClient::State::kUnRegistered);

    auto is_localuser = std::dynamic_pointer_cast<ILocalUser>(*client);
    ASSERT_EQ(is_localuser, nullptr);

    client_database->RegisterLocalUser(uuid_client1);

    client = client_database->GetClient(uuid_client1);

    ASSERT_EQ((*client)->GetType(), IClient::Type::kLocalUser);
    is_localuser = std::dynamic_pointer_cast<ILocalUser>(*client);
    ASSERT_EQ(is_localuser->GetUUID(), uuid_client1);
}