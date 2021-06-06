#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "RemoteUser.h"
#include "MockServer.h"

using ::testing::AtLeast;
using ::testing::Throw;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::_;

class RemoteUserTests : public ::testing::Test
{
    public:
    MockServer mock_server1;
    std::unique_ptr<RemoteUser> remote_user1;

    std::string nickname1;
    std::string username1;
    std::string realname1;

	std::string remote_server_name;

    void SetUp() override
    {
        nickname1 = "nickname1";
        username1 = "username1";
        realname1 = "realname1";

		remote_server_name = "remote.server.com";

		EXPECT_CALL(mock_server1, GetServerName())
			.WillRepeatedly(ReturnRef(remote_server_name));
        remote_user1 = std::make_unique<RemoteUser>(&mock_server1, &mock_server1, nickname1, username1, realname1);
    }
};

TEST_F(RemoteUserTests, todo)
{
    remote_user1->Push("hallo");
}
