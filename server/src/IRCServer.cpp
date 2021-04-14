#include "IRCServer.h"
#include "Client.h"
#include "Parser.h"
#include "RawMessage.h"
#include "Message.h"
#include "TCPIOHandler.h"
#include "MessageDispatcher.h"
#include "Socket.h"
#include "Numerics.h"
#include "Utilities.h"

IRCServer::IRCServer(const std::string &config_path) :
    server_data_(std::make_unique<ServerData>(config_path)),
    message_dispatcher_(std::make_unique<MessageDispatcher>(server_data_.get())),
    logger("IRCServer")
{}

IRCServer::~IRCServer()
{}

auto IRCServer::Start() -> void
{
    logger.Log(LogLevel::INFO, "Attempting to start server...");

	server_data_->client_database_.SetConfig(&server_data_->server_config_);

    TCP::AddressInfo address_info(server_data_->server_config_.GetAddress(),
		server_data_->server_config_.GetPort());

    auto server_socket = std::make_shared<TCP::Socket>();
    server_socket->Listen(address_info);

    tcp_io_controller_.AddSocket(server_socket);

    logger.Log(LogLevel::INFO, "Server started!");
};

auto IRCServer::RunOnce() -> void
{
    tcp_io_controller_.RunOnce();

    tcp_io_controller_.AcceptNewConnections(
        [this](std::shared_ptr<TCP::ISocket> socket)
        {
            auto io_handler = std::make_unique<TCPIOHandler>(socket);
            auto client = std::make_unique<Client>(std::move(io_handler));

            server_data_->client_database_.AddClient(std::move(client));
            logger.Log(LogLevel::DEBUG, "New client on FD: %d", socket->GetFD());
        });

    server_data_->client_database_.PollClients(
        [this](IClient* client, std::string raw_message)
        {
            try {
                auto parsed_message = IRC::Parser::RunParser<IRC::RawMessage>(IRC::ParseRawMessage, raw_message);
                auto message = Message(client->GetUUID(), parsed_message);

                logger.Log(LogLevel::DEBUG, "Received: %s", raw_message.c_str());
                if (!message_dispatcher_->Dispatch(message))
                    client->Push(GetErrorMessage(ERR_UNKNOWNCOMMAND, message.GetCommand()));

            } catch (ParseException &e) {
                logger.Log(LogLevel::WARNING, "Failed to parse: %s ", raw_message.c_str());
                client->Push("ERROR :Could not parse message");
            }
        });
    server_data_->client_database_.SendAll();
}
