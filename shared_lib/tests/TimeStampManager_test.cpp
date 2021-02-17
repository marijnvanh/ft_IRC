#include "unistd.h"
#include "Socket.h"
#include "TimeStamps.h"
#include "gtest/gtest.h"

using namespace IRC;

class TimeStampsTests : public ::testing::Test
{
    public:
	TimeStamps timeStamps;

    void SetUp() override
    {
		
    }
    void TearDown() override
    {
    }
};

TEST_F(TimeStampsTests, CreateAndClearTimeStamp)
{
	timeStamps.Add("Time stamp test - 01");

	timeStamps.PrintAll();
}
