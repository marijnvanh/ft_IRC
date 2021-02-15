#include "Server.h"
#include "Client.h"
#include "Parser.h"
#include "RawMessage.h"
#include "Message.h"
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
        [this](IRC::UUID uuid, std::string raw_message)
        {
            std::cout << "Received message from user with uuid: " << uuid << std::endl;
            std::cout << "Message: " << raw_message << std::endl;

            using namespace IRC::Parser;

            try {
                auto parsed = RunParser<IRC::RawMessage>(IRC::ParseRawMessage, raw_message);
                auto message = Message(uuid, parsed);

                std::cout << message << std::endl;

                (void)parsed;
                (void)message;
            } catch (ParseException &e) {
                std::cout << "Failed to parse message" << std::endl;
            }
            //if parse succesful
            //call handler
            //else
            //return invalid message
            auto client = client_database_.GetClient(uuid);
            client->Access([&raw_message](IClient &client)
            {
                client.Push(std::make_shared<std::string>("ACK: " + raw_message));
            });
        });
    client_database_.SendAll();
}
