#include "Resolver.h"
#include "Socket.h"
#include "IOController.h"

#include <queue>
#define PORT "5000"

int main(int argc, char *argv[])
{
    if (argc != 2)
        exit(1);
    std::string server_address(argv[1]);
    try {
        TCP::AddressInfo address_info = TCP::Resolver::Resolve(server_address, PORT);
        std::queue<std::string> message_queue;
        TCP::IOController io_controller(address_info, message_queue);
        while (1)
        {
            std::cout << "Poll once" << std::endl;
            io_controller.RunOnce(5);
        }
    }
    catch (TCP::Resolver::Error &ex)
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
