#include "gtest/gtest.h"
#include "Mutex.hpp"
#include <thread>
#include <chrono>

TEST(MutexTest, MultiThreadLock)
{
    ft_irc::Mutex<int> mtx = ft_irc::MakeMutex<int>(42);

    { ASSERT_EQ(*mtx.Take(), 42); }

    std::thread t1([&mtx]() {
        { ASSERT_EQ(*mtx.Take(), 42); }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            auto h = mtx.Take();
            ASSERT_EQ(mtx.IsLocked(), true);
            std::cout << "Thread 1 took lock..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            (*h)++;
        }
        std::cout << "Thread 1 released lock..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
    std::thread t2([&mtx]() {
        { ASSERT_EQ(*mtx.Take(), 42); }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        {
            auto h = mtx.Take();
            ASSERT_EQ(mtx.IsLocked(), true);
            std::cout << "Thread 2 took lock..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            (*h)++;
        }
        std::cout << "Thread 2 released lock..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
    t1.join();
    t2.join();

    ASSERT_EQ(mtx.IsLocked(), false);

    { ASSERT_EQ(*mtx.Take(), 44); }
}
