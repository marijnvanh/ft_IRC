#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "unistd.h"
#include "IOController.h"
#include "Socket.h"

using namespace IRC::TCP;

class IOControllerTests : public ::testing::Test
{
public:
    std::shared_ptr<IOController> io_controller;
	std::shared_ptr<Socket> socket;

    void SetUp() override
    {
		socket = std::make_shared<Socket>();
        io_controller = std::make_shared<IOController>();
    }
    void TearDown() override
    {
    }
};

TEST_F(IOControllerTests, AddAndRemoveSocket)
{
	io_controller->AddSocket(socket);

	ASSERT_EQ(io_controller->GetSocketsCount(), 1);

	io_controller->RemoveSocket(socket);

	ASSERT_EQ(io_controller->GetSocketsCount(), 0);
}

TEST_F(IOControllerTests, AddAndRemoveInvalidSocketAfterRunOnce)
{
	io_controller->AddSocket(socket);

	ASSERT_EQ(io_controller->GetSocketsCount(), 1);

	socket->SetState(SocketState::kDisconnected);
	io_controller->RunOnce();

	ASSERT_EQ(io_controller->GetSocketsCount(), 0);
}
