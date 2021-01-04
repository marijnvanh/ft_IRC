#include "AddressInfo.h"
#include "Socket.h"
#include "IOController.h"
#include "Message.h"

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
        TCP::IOController io_controller(address_info, read_queue, send_queue);
        while (1)
        {
            std::cout << "Poll once" << std::endl;
            io_controller.RunOnce(5);
            

            while (read_queue.empty() == false)
            {
                auto message = read_queue.front();
                if (message.GetData() != "")
                {
                    std::cout << "Received: " << message.GetData() << std::endl;
                    TCP::Message response(message.GetSocket(), "ACK: " + message.GetData());
                    send_queue.push(std::move(response));
                }
                read_queue.pop(); 
            }
            if (send_queue.empty() == false)
            {
                std::cout << send_queue.front();
            }
            sleep(1);
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
