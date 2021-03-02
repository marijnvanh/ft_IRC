#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Client.h"
#include "Server.h"
#include "MockIOHandler.h"
#include "MockClient.h"
#include "MockServer.h"
#include <memory>

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::_;

class ServerTests : public ::testing::Test
{
    public:
    std::unique_ptr<MockIOHandler> unique_io_handler;
    std::shared_ptr<MockServer> mock_server;
    MockIOHandler *io_handler;
    std::shared_ptr<Client> client;

    void SetUp() override
    {
        mock_server = std::make_shared<MockServer>();
        unique_io_handler = std::make_unique<MockIOHandler>();
        io_handler = unique_io_handler.get();
        client = std::make_shared<Client>(std::move(unique_io_handler), mock_server);
    }
};

//TODO when we implement server stuff
TEST_F(ServerTests, Todo)
{
    Server server(std::move(*client.get()));
}
