#include "gtest/gtest.h"
#include "Socket.h"
#include "AddressInfo.h"
#include "IOController.h"
#include "unistd.h"
#include <queue>

#define TEST_PORT "5000"
#define TEST_LOCAL "localhost"

void * LaunchServer(void * ptr);  

class IOControllerIntegrationTest : public ::testing::Test
{
    public:
    pthread_t server_thread;
    std::queue<TCP::Message> send_queue;
    std::queue<TCP::Message> read_queue;
    std::shared_ptr<TCP::Socket>client_socket;
    std::shared_ptr<TCP::IOController>client_io_controller;

    void SetUp() override
    {
        ::pthread_create(&server_thread, nullptr, LaunchServer, nullptr);

        sleep(1);
        TCP::AddressInfo address_info_client(TEST_LOCAL, TEST_PORT);
        client_socket = std::make_shared<TCP::Socket>();
        client_socket->Connect(address_info_client);

        client_io_controller = std::make_shared<TCP::IOController>(client_socket, read_queue, send_queue);
    }
    void TearDown() override
    {
        TCP::Message exit_message(client_socket, "exit");
        send_queue.push(std::move(exit_message));
        client_io_controller->RunOnce();
        sleep(1);
        ::pthread_join(server_thread, nullptr);        
    }
};


TEST_F(IOControllerIntegrationTest, SendMessage)
{

    TCP::Message test_message(client_socket, "test");
    send_queue.push(std::move(test_message));
    client_io_controller->RunOnce();
    sleep(5); //TODO fix time out randomness
    client_io_controller->RunOnce();
    ASSERT_FALSE(read_queue.empty());
    auto message = read_queue.front();
    ASSERT_EQ("ACK: test", *message.GetData());
}

void *LaunchServer(void * ptr)
{
    (void)ptr;
    std::queue<TCP::Message> send_queue;
    std::queue<TCP::Message> read_queue;
    
    TCP::AddressInfo address_info_server(TEST_LOCAL, TEST_PORT);
    TCP::IOController server_io_controller(address_info_server, read_queue, send_queue);

    while (1)
    {
        sleep(1);
        server_io_controller.RunOnce();
        if (read_queue.empty() == false)
        {
            auto message = read_queue.front();
            if (*message.GetData() == "exit")
                break ;
            if (*message.GetData() != "")
            {
                TCP::Message response(message.GetSocket(), "ACK: " + *message.GetData());
                send_queue.push(std::move(response));
            }
            read_queue.pop();
        }
    }
    return (0);
}
