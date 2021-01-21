#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "Client.h"
#include "IIOHandler.h"
#include <memory>

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::_;

class MockIOHandler : public ft_irc::IIOHandler {
    public:

    MOCK_METHOD1(Send, void(std::string msg));
    MOCK_METHOD0(Receive, std::string());
};

class ClientTests : public ::testing::Test
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

TEST_F(ClientTests, SendAllWithMultipleMessagesInQueue)
{
    client->Push(std::make_shared<std::string>("test1"));
    client->Push(std::make_shared<std::string>("test2"));
    client->Push(std::make_shared<std::string>("test3"));
    
    EXPECT_CALL(*io_handler, Send("test1"))
        .Times(1);
    EXPECT_CALL(*io_handler, Send("test2"))
        .Times(1);
    EXPECT_CALL(*io_handler, Send("test3"))
        .Times(1);
    client->SendAll();
}

TEST_F(ClientTests, SendAllFailedToSendException)
{
    client->Push(std::make_shared<std::string>("test"));

    EXPECT_CALL(*io_handler, Send("test"))
        .Times(1)
        .WillOnce(Throw(ft_irc::IIOHandler::FailedToSend("test")));

    client->SendAll();

    /* Make sure that the message is re-send */
    EXPECT_CALL(*io_handler, Send("test"))
        .Times(1);
    client->SendAll();
}

TEST_F(ClientTests, SendAllClosedIOHandlerException)
{
    client->Push(std::make_shared<std::string>("test"));

    EXPECT_CALL(*io_handler, Send("test"))
        .Times(1)
        .WillOnce(Throw(ft_irc::IIOHandler::Closed("test")));

    ASSERT_THROW(client->SendAll(), IClient::Disconnected);
}

TEST_F(ClientTests, ReceiveMessage)
{
    EXPECT_CALL(*io_handler, Receive())
        .Times(1)
        .WillOnce(Return("test"));

    ASSERT_EQ(client->Receive(), "test");
}

TEST_F(ClientTests, ReceiveFailedToReceiveException)
{
    EXPECT_CALL(*io_handler, Receive())
        .Times(1)
        .WillOnce(Throw(ft_irc::IIOHandler::FailedToReceive("test")));
    
    ASSERT_EQ(client->Receive(), std::nullopt);
}

TEST_F(ClientTests, ReceiveClosedIOHandlerException)
{
    EXPECT_CALL(*io_handler, Receive())
        .Times(1)
        .WillOnce(Throw(ft_irc::IIOHandler::Closed("test")));

    ASSERT_THROW(client->Receive(), IClient::Disconnected);
}
