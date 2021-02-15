#include "gtest/gtest.h"
#include "unistd.h"
#include "IOController.h"
#include "Socket.h"

using namespace IRC::TCP;

class IOControllerTests : public ::testing::Test
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