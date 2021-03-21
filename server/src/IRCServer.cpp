#include "IRCServer.h"
#include "Client.h"
#include "Parser.h"
#include "RawMessage.h"
#include "Message.h"
#include "TCPIOHandler.h"
#include "MessageDispatcher.h"
#include "LogSettings.h"

IRCServer::IRCServer(const std::string &config_path) :
    server_data_(std::make_unique<ServerData>(config_path)),
    message_dispatcher_(std::make_unique<MessageDispatcher>(server_data_.get())),
    logger("IRCServer")
{
	server_data_->server_config_.TryParseFrom(config_path);

    SetLogSettings();
}

IRCServer::~IRCServer()
{}

auto IRCServer::Start() -> void
{
    logger.Log(LogLevel::INFO, "Attempting to start server...");

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
        [this](std::shared_ptr<TCP::Socket> socket)
        {
            auto io_handler = std::make_unique<TCPIOHandler>(socket);
            auto client = std::make_unique<Client>(std::move(io_handler));

            server_data_->client_database_.AddClient(std::move(client));
            logger.Log(LogLevel::DEBUG, "New client on FD: ", socket->GetFD());
        });

    server_data_->client_database_.PollClients(
        [this](IClient* client, std::string raw_message)
        {
            try {
                auto parsed_message = IRC::Parser::RunParser<IRC::RawMessage>(IRC::ParseRawMessage, raw_message);
                auto message = Message(client->GetUUID(), parsed_message);

                std::cout << message << std::endl;
                message_dispatcher_->Dispatch(message);

            } catch (ParseException &e) {
                logger.Log(LogLevel::WARNING, "Failed to parse: %s ", raw_message.c_str());
                //TODO Handle invalid message
            }
        });
    server_data_->client_database_.SendAll();
}
