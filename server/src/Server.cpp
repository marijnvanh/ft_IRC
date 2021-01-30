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

    tcp_io_controller_.AddSocket(server_socket);

    std::cout << "Server started!" << std::endl;
};

auto Server::RunOnce() -> void
{
    tcp_io_controller_.RunOnce();

    tcp_io_controller_.AcceptNewConnections(
        [this](std::shared_ptr<TCP::Socket> socket)
        {
            auto io_handler = std::make_unique<TCPIOHandler>(socket);
            auto client = std::make_unique<Client>(std::move(io_handler));
            client_database_.AddClient(std::move(client));
            std::cout << "New client on FD: " << socket->GetFD() << std::endl;
        });

    client_database_.PollClients(
        [](int uuid, std::string message)
        {
            std::cout << "Received message from user wiht uuid: " << uuid << std::endl;
            std::cout << "Message: " << message << std::endl;

            //TODO Parse
            //if parse succesful
            //call handler
            //else
            //return invalid message
        });
    client_database_.SendAll();
}
