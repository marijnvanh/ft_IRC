#include "gtest/gtest.h"
#include "unistd.h"
#include "TCPIOHandler.h"
#include "Socket.h"

using namespace IRC;

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
    TCP::TCPIOHandler io_handler(socket);

    ASSERT_THROW(io_handler.Receive(), IRC::IIOHandler::Closed);
}

TEST_F(TCPIOHandlerTests, FailToSendToClosedSocket)
{
    TCP::TCPIOHandler io_handler(socket);

    ASSERT_THROW(io_handler.Send("Test"), IRC::IIOHandler::Closed);
}
