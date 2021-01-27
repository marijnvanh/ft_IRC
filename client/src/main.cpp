#include "AddressInfo.h"
#include "IOController.h"

#include <queue>
#include <string>
#include <fcntl.h>
#include <unistd.h>

#define PORT "5000"

using namespace IRC;

int main(int argc, char *argv[])
{
    if (argc != 2)
        exit(1);

    std::string server_address(argv[1]);

	std::cout << "Attempting to start client..." << std::endl;

    try
	{
        TCP::AddressInfo address_info(server_address, PORT);
        TCP::IOController io_controller;

        auto client_socket = std::make_shared<TCP::Socket>();
        client_socket->Connect(address_info);

		io_controller.AddSocket(client_socket);
        
		std::string line;

		for (;;)
		{
			io_controller.RunOnce();

			int c = std::cin.peek();
			if (c != 0 && c != EOF && std::getline(std::cin, line))
			{
				// Send to remote.
				client_socket->Send(line);
			}

			// Check to see if socket can be read.
			if (client_socket->GetState() == TCP::SocketState::kReadyToRead)
			{
				std::cout << client_socket->Recv();
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
    catch (TCP::IOController::Error &ex)
    {
        std::cout << "IOControler error: " << ex.what() << std::endl;
        exit(-1);
    }

    return (0);
}
