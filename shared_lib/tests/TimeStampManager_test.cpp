#include "gtest/gtest.h"
#include "unistd.h"
#include "TimeStampManager.h"
#include "Socket.h"

using namespace IRC;

class TimeStampManagerTests : public ::testing::Test
{
    public:
	TimeStampManager timeStampManager;

    void SetUp() override
    {
		
    }
    void TearDown() override
    {
    }
};

TEST_F(TimeStampManagerTests, CreateAndClearTimeStamp)
{
	timeStampManager.AddNew("Time stamp test - 01");

	timeStampManager.PrintAll();
}
