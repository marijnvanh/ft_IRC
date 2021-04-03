#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "RemoteUser.h"
#include "MockServer.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::_;

class RemoteUserTests : public ::testing::Test
{
    public:
    MockServer mock_server1;
    std::unique_ptr<RemoteUser> remote_user1;

    std::string nickname1;
    std::string username1;
    std::string realname1;

    void SetUp() override
    {
        nickname1 = "nickname1";
        username1 = "username1";
        realname1 = "realname1";
        remote_user1 = std::make_unique<RemoteUser>(&mock_server1, &mock_server1, nickname1, username1, realname1);
    }
};

//TODO
TEST_F(RemoteUserTests, todo)
{
    remote_user1->Push("hallo");
}
