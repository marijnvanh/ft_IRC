#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IOController.h"
#include "Socket.h"

using namespace IRC;

class IOControllerTests : public ::testing::Test
{
public:
    std::shared_ptr<TCP::IOController> io_controller;
	std::shared_ptr<TCP::Socket> socket;

    void SetUp() override
    {
		socket = std::make_shared<TCP::Socket>();
        io_controller = std::make_shared<TCP::IOController>();
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

	socket->SetState(TCP::SocketState::kDisconnected);
	io_controller->RunOnce();

	ASSERT_EQ(io_controller->GetSocketsCount(), 0);
}
