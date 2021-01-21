#include "gtest/gtest.h"
#include "Mutex.h"
#include <thread>
#include <chrono>

TEST(MutexTest, MultiThreadLock)
{
    ft_irc::Mutex<int> mtx = ft_irc::MakeMutex<int>(42);

    { ASSERT_EQ(*mtx.Take(), 42); }

    std::thread t1([&mtx]() {
        { ASSERT_EQ(*mtx.Take(), 42); }
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        {
            auto h = mtx.Take();
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
            std::cout << "Thread 2 took lock..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            (*h)++;
        }
        std::cout << "Thread 2 released lock..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
    t1.join();
    t2.join();

    { ASSERT_EQ(*mtx.Take(), 44); }
}

TEST(MutexTest, TryLockFailure)
{
    ft_irc::Mutex<int> mtx = ft_irc::MakeMutex<int>(0);
    { 
        auto first_handle = mtx.TryTake();
        auto second_handle = mtx.TryTake();
        ASSERT_EQ(second_handle, nullptr);
        (**first_handle) = 123;
    }
}

TEST(MutexTest, Printing)
{
    ft_irc::Mutex<int> mtx = ft_irc::MakeMutex<int>(42);
    std::ostringstream stream;

    stream << mtx;
    ASSERT_EQ(stream.str(), "Mutex(42)");

    auto h = mtx.Take();
    std::ostringstream stream2;

    stream2 << mtx;
    ASSERT_EQ(stream2.str(), "Mutex(<<locked>>)");
}
