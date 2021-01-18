#include "gtest/gtest.h"
#include "unistd.h"
#include "TCPIOHandler.h"
#include "Socket.h"

class TCPIOHandlerTests : public ::testing::Test
{
    public:
    std::shared_ptr<TCP::Socket>socket;

    void SetUp() override
    {
        socket = std::make_shared<TCP::Socket>();
    }
    void TearDown() override
    {
    }
};

TEST_F(TCPIOHandlerTests, FailToReadFromClosedSocket)
{
    ft_irc::TCPIOHandler io_handler(socket);

    ASSERT_THROW(io_handler.Receive(), ft_irc::IIOHandler::Closed);
}

TEST_F(TCPIOHandlerTests, FailToSendToClosedSocket)
{
    ft_irc::TCPIOHandler io_handler(socket);

    ASSERT_THROW(io_handler.Send("Test"), ft_irc::IIOHandler::Closed);
}
