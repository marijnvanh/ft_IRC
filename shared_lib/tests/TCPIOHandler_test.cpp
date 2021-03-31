#include "gtest/gtest.h"
#include "unistd.h"
#include "TCPIOHandler.h"
#include "Socket.h"
#include "MockSocket.h"

using ::testing::Return;
using ::testing::Throw;
using ::testing::_;
using namespace IRC;

class TCPIOHandlerTests : public ::testing::Test
{
    public:
    std::shared_ptr<MockSocket>socket;
    MockSocket *raw_socket;

    void SetUp() override
    {
        socket = std::make_shared<MockSocket>();
        raw_socket = socket.get();
    
        EXPECT_CALL(*raw_socket, Close());
    }
    void TearDown() override
    {
    }
};

TEST_F(TCPIOHandlerTests, ReadFromSocket)
{
    TCPIOHandler io_handler(socket);

    socket->SetState(TCP::SocketState::kReadyToRead);
    EXPECT_CALL(*raw_socket, Recv())
        .WillOnce(Return("test\r\n"));

    ASSERT_EQ(io_handler.Receive(), "test");
}

TEST_F(TCPIOHandlerTests, FailToReadFromClosedSocket)
{
    TCPIOHandler io_handler(socket);

    socket->SetState(TCP::SocketState::kDisconnected);

    ASSERT_THROW(io_handler.Receive(), IIOHandler::Closed);
}

TEST_F(TCPIOHandlerTests, FailToSendToClosedSocket)
{
    TCPIOHandler io_handler(socket);

    EXPECT_CALL(*raw_socket, Send(_))
        .WillOnce(Throw(IRC::TCP::ISocket::Closed()));
    
    ASSERT_THROW(io_handler.Send("Test"), IIOHandler::Closed);
}
