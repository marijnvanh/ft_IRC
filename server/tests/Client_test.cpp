#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Client.h"
#include "MockIOHandler.h"
#include "MockServer.h"
#include <memory>

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::_;

class ClientTests : public ::testing::Test
{
    public:
    std::unique_ptr<MockIOHandler> unique_io_handler;
    MockIOHandler *io_handler;
    std::shared_ptr<Client> client;
    std::shared_ptr<MockServer> mock_server;

    void SetUp() override
    {
        mock_server = std::make_shared<MockServer>();
        unique_io_handler = std::make_unique<MockIOHandler>();
        io_handler = unique_io_handler.get();
        client = std::make_shared<Client>(std::move(unique_io_handler), mock_server);
    }
};

TEST_F(ClientTests, SendAllWithMultipleMessagesInQueue)
{
    client->Push("test1");
    client->Push("test2");
    client->Push("test3");
    
    EXPECT_CALL(*io_handler, Send("test1"))
        .Times(1);
    EXPECT_CALL(*io_handler, Send("test2"))
        .Times(1);
    EXPECT_CALL(*io_handler, Send("test3"))
        .Times(1);
    client->SendAll();

    /* Test if queue is empty */
    EXPECT_CALL(*io_handler, Send(_))
        .Times(0);
    client->SendAll();
}

TEST_F(ClientTests, SendAllFailedToSendException)
{
    client->Push("test");

    EXPECT_CALL(*io_handler, Send("test"))
        .Times(1)
        .WillOnce(Throw(IRC::IIOHandler::FailedToSend("test")));

    client->SendAll();

    /* Make sure that the message is re-send */
    EXPECT_CALL(*io_handler, Send("test"))
        .Times(1);
    client->SendAll();
}

TEST_F(ClientTests, SendAllClosedIOHandlerException)
{
    client->Push("test");

    EXPECT_CALL(*io_handler, Send("test"))
        .Times(1)
        .WillOnce(Throw(IRC::IIOHandler::Closed("test")));

    ASSERT_THROW(client->SendAll(), IClient::Disconnected);
}

TEST_F(ClientTests, ReceiveMessage)
{
    EXPECT_CALL(*io_handler, Receive())
        .Times(1)
        .WillOnce(Return(std::optional<std::string>("test")));

    ASSERT_EQ(client->Receive(), "test");
}

TEST_F(ClientTests, NothingToReceive)
{
    EXPECT_CALL(*io_handler, Receive())
        .Times(1)
        .WillOnce(Return(std::nullopt));
    
    ASSERT_EQ(client->Receive(), std::nullopt);
}

TEST_F(ClientTests, ReceiveClosedIOHandlerException)
{
    EXPECT_CALL(*io_handler, Receive())
        .Times(1)
        .WillOnce(Throw(IRC::IIOHandler::Closed("test")));

    ASSERT_THROW(client->Receive(), IClient::Disconnected);
}
