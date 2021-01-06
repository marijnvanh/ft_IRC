#include "gtest/gtest.h"
#include "Socket.h"
#include "AddressInfo.h"
#include "IOController.h"
#include "unistd.h"
#include <queue>

#define TEST_PORT "5000"
#define TEST_LOCAL "localhost"

void * serverfunc(void * ptr);  
void serverfunc1(void * ptr);  
void * clientfunc(void * ptr);    


TEST(IOController, test_everything)
{
  pthread_t mThreadID1, mThreadID2;
  ::pthread_create(&mThreadID1, nullptr, serverfunc, nullptr);
  ::sleep(1);
  ::pthread_create(&mThreadID2, nullptr, clientfunc, nullptr);
  ::pthread_join(mThreadID1, nullptr);
  ::pthread_join(mThreadID2, nullptr);        
}

void *serverfunc(void * ptr)
{
    serverfunc1(ptr);
    return 0;
}
void serverfunc1(void * ptr)
{
    (void)ptr;
    std::queue<TCP::Message> send_queue;
    std::queue<TCP::Message> read_queue;
    
    TCP::AddressInfo address_info_server(TEST_LOCAL, TEST_PORT);
    TCP::IOController server_io_controller(address_info_server, read_queue, send_queue);

    server_io_controller.RunOnce();

    sleep(5);
    server_io_controller.RunOnce();
    sleep(2);
    server_io_controller.RunOnce();
    read_queue.pop();
    ASSERT_FALSE(read_queue.empty());
    auto message = read_queue.front();
    ASSERT_EQ("test", *message.GetData());
}

void *clientfunc(void * ptr)
{
    (void)ptr;
    std::queue<TCP::Message> send_queue;
    std::queue<TCP::Message> read_queue;
    TCP::AddressInfo address_info_client(TEST_LOCAL, TEST_PORT);
    auto client_socket = std::make_shared<TCP::Socket>();
    client_socket->Connect(address_info_client);
    TCP::IOController client_io_controller(client_socket, read_queue, send_queue);

    TCP::Message test_message(client_socket, "test");
    send_queue.push(std::move(test_message));
    client_io_controller.RunOnce();
    return 0;
}