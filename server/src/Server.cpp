#include "Server.h"
#include "Client.h"
#include "TCPIOHandler.h"

Server::Server()
{}

Server::~Server()
{}


auto Server::Start(std::string address) -> void
{
    std::cout << "Attempting to start server..." << std::endl;

    TCP::AddressInfo address_info(address, PORT);

    auto server_socket = std::make_shared<TCP::Socket>();
    server_socket->Listen(address_info);

    io_controller_.AddSocket(server_socket);

    std::cout << "Server started!" << std::endl;
};

auto Server::RunOnce() -> void
{
    io_controller_.RunOnce();

    io_controller_.AcceptNewConnections(
        [=](std::shared_ptr<TCP::Socket> socket)
        {
            auto io_handler = std::make_unique<TCPIOHandler>(socket);
            auto client = std::make_unique<Client>(std::move(io_handler));
            client_database_.AddClient(std::move(client));
            std::cout << "New client on FD: " << socket->GetFD() << std::endl;
        });

    client_database_.PollClients(
        [=](std::string message)
        {
            std::cout << "Received message: " << message << std::endl;
        });
}
