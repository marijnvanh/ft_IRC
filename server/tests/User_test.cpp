#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Client.h"
#include "MockIOHandler.h"
#include "User.h"
#include "MockClient.h"
#include <memory>

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::_;

class FakeUser : public MockClient, User
{
    public:

};

class UserTests : public ::testing::Test
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

TEST_F(UserTests, todo)
{
    FakeUser hello;
}
