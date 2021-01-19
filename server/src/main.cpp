#include "Socket.h"
#include "Message.h"
#include "AddressInfo.h"
#include "IOController.h"

#include <queue>
#include <memory>
#include <unistd.h>
#define PORT "5000"

std::vector<std::shared_ptr<TCP::Socket>> ClientSockets;

static void AcceptClient(std::shared_ptr<TCP::Socket> socket)
{
	// Do something?
	std::cout << "New client on FD: " << socket->GetFD() << std::endl;

	ClientSockets.push_back(socket);
}

int main(int argc, char *argv[])
{
    using namespace ft_irc::parser;

    std::cout << "Parsing section" << std::endl;

    std::string source(":emiflake@nixflake PRIVMSG #ft-irc :hello, how are you");
    CharStream cs = CharStream::FromString(source);

    auto message = ft_irc::ParseRawMessage(cs);

    std::cout << "Command: " << message.command.name << std::endl;
    std::cout << "........." << std::endl;

    if (argc != 2)
        exit(1);

    std::string server_address(argv[1]);

	std::cout << "Attempting to start server..." << std::endl;

    try
	{
        TCP::AddressInfo address_info(server_address, PORT);
        TCP::IOController io_controller;
		
		auto server_socket = std::make_shared<TCP::Socket>();
		server_socket->Listen(address_info, 20, false);

		io_controller.AddSocket(server_socket);
        
		while (1)
        {
            io_controller.RunOnce();

			io_controller.AcceptNewConnections(AcceptClient);

			// Loop through connected clients.
			for (std::vector<std::shared_ptr<TCP::Socket>>::iterator it = ClientSockets.begin(); it != ClientSockets.end();)
			{
				if ((*it)->GetState() == TCP::SocketState::kDisconnected)
				{
					it = ClientSockets.erase(it);
				}
				else
				{
					if ((*it)->GetState() == TCP::SocketState::kReadyToRead)
					{
						std::cout << (*it)->Recv() << std::endl;
					}
					++it;
				}
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
