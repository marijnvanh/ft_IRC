#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Client.h"
#include "Server.h"
#include "MockIOHandler.h"
#include "MockClient.h"
#include <memory>

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::_;

class ServerTests : public ::testing::Test
{
    public:
    std::unique_ptr<MockIOHandler> unique_io_handler;
    MockIOHandler *io_handler;
    std::shared_ptr<Client> client;

    void SetUp() override
    {
        unique_io_handler = std::make_unique<MockIOHandler>();
        io_handler = unique_io_handler.get();
        client = std::make_shared<Client>(std::move(unique_io_handler));
    }
};

//TODO when we implement server stuff
#include "MockServer.h"
TEST_F(ServerTests, Todo)
{
    MockServer();
    Server server(std::move(*client.get()));
}
