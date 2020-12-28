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
        /* Client */
        TCP::AddressInfo address_info = TCP::Resolver::Resolve(server_address, PORT);
        TCP::Socket socket;
        socket.Connect(address_info);
        socket.Send("Some data");
        std::cout << socket.ToStr() << std::endl;
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
