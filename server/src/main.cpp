#include "Socket.h"
#include "Message.h"
#include "AddressInfo.h"
#include "IOController.h"

#include <queue>
#include <memory>
#include <unistd.h>
#define PORT "5000"

using namespace IRC;

class Server
{
public:
	Server(){};
	~Server(){};

	void Start(std::string address) {
		std::cout << "Attempting to start server..." << std::endl;

        TCP::AddressInfo address_info(address, PORT);

		auto server_socket = std::make_shared<TCP::Socket>();
		server_socket->Listen(address_info, 20, false);

		io_controller_.AddSocket(server_socket);

		std::cout << "Server started!" << std::endl;
	};

	void Run() {
		io_controller_.RunOnce();

		// Lambda/std::function expression for anonymous newly accepted socket handling.
		io_controller_.AcceptNewConnections(
			[=](std::shared_ptr<TCP::Socket> socket) {
				std::cout << "New client on FD: " << socket->GetFD() << std::endl;

				client_sockets_.push_back(socket);
		});

		for (std::vector<std::shared_ptr<TCP::Socket>>::iterator it = client_sockets_.begin(); it != client_sockets_.end();)
		{
			try
			{
				if ((*it)->GetState() == TCP::SocketState::kReadyToRead)
				{
					std::cout << (*it)->Recv() << std::endl;
				}
				++it;
			}
			catch(TCP::Socket::Closed &ex)
			{
				std::cout << "Client left with FD: " << (*it)->GetFD() << std::endl;
				it = client_sockets_.erase(it);
			}
		}
	}

private:
	std::vector<std::shared_ptr<TCP::Socket>> client_sockets_;

	TCP::IOController io_controller_;
};

int main(int argc, char *argv[])
{
    if (argc != 2)
        exit(1);

    std::string server_address(argv[1]);

    try
	{
        Server server;

		server.Start(std::move(server_address));

		while (1)
        {
			server.Run();

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
