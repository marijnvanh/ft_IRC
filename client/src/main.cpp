#include "AddressInfo.h"
#include "IOController.h"
#include <string>
#include <queue>
#include <unistd.h>
#define PORT "5000"

int main(int argc, char *argv[])
{
    if (argc != 2)
        exit(1);
    std::string server_address(argv[1]);
    try {
        TCP::AddressInfo address_info(server_address, PORT);
        std::queue<TCP::Message> send_queue;
        std::queue<TCP::Message> read_queue;
        auto client_socket = std::make_shared<TCP::Socket>();
        client_socket->Connect(address_info);
        TCP::IOController io_controller(client_socket, read_queue, send_queue);
        
        for (std::string line; std::getline(std::cin, line);)
        {
            std::cout << "Run once and send: " << line << std::endl;
            TCP::Message message(client_socket, std::move(line));
            send_queue.push(std::move(message));
            io_controller.RunOnce();
            sleep(2);
            io_controller.RunOnce();
            while (read_queue.empty() == false)
            {
                auto recv_message = read_queue.front();
                std::cout << "Received: " << recv_message.GetData() << std::endl;
                read_queue.pop();
            }
        }
    }
    catch (TCP::AddressInfo::ResolveError &ex)
    {
        std::cout << "Resolve error: " << ex.what() << std::endl;
        exit(-1);
    }
    catch (TCP::Socket::Error &ex)
    {
        std::cout << "Socket error: " << ex.what() << std::endl;
        exit(-1);
    }

    return (0);
}
