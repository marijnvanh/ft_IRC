#include "Server.h"
#include "Client.h"
#include "Parser.h"
#include "RawMessage.h"
#include "Message.h"
#include "TCPIOHandler.h"
#include "MessageDispatcher.h"

Server::Server() :
    server_data_(std::make_shared<ServerData>()),
    message_dispatcher_(std::make_unique<MessageDispatcher>(server_data_))
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
            server_data_->client_database_->AddClient(std::move(client));
            std::cout << "New client on FD: " << socket->GetFD() << std::endl;
        });

    server_data_->client_database_->PollClients(
        [this](std::shared_ptr<IRC::Mutex<IClient>> client, const std::string raw_message)
        {
            auto cs = IRC::Parser::CharStream(std::make_shared<std::string>(raw_message));
            try {
                std::cout << "IN:     \"" << raw_message << "\"" << std::endl;
                auto parsed_message = IRC::ParseRawMessage(cs);

                std::cout << "PARSED: \"" << parsed_message << "\"" << std::endl;
                auto message = Message(client, parsed_message);

                std::cout << message << std::endl;
                message_dispatcher_->Dispatch(message);

            } catch (ParseException &e) {
                e.Explain(cs);
                //TODO Handle invalid message
            }
        });
    server_data_->client_database_->SendAll();
}
