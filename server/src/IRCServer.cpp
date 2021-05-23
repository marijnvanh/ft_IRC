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
#ifdef ENABLE_SSL
    #include "SSL.h"
#endif

IRCServer::IRCServer(const std::string &config_path) :
    server_data_(std::make_unique<ServerData>(config_path)),
    logger("IRCServer")
{
    message_dispatcher_ = std::make_unique<MessageDispatcher>(server_data_.get(), this);

}

IRCServer::~IRCServer()
{}

auto IRCServer::Start() -> void
{
    logger.Log(LogLevel::INFO, "Attempting to start server...");
#ifdef ENABLE_SSL
    TCP::InitSSL(server_data_->server_config_.GetSSLcrt(), server_data_->server_config_.GetSSLkey());
#endif

	server_data_->client_database_.SetConfig(&server_data_->server_config_);

    TCP::AddressInfo address_info(server_data_->server_config_.GetAddress(),
		server_data_->server_config_.GetPort());
    auto server_socket = std::make_shared<TCP::Socket>(false);
    server_socket->Listen(address_info);
    tcp_io_controller_.AddSocket(server_socket);

#ifdef ENABLE_SSL
    TCP::AddressInfo address_info_ssl(server_data_->server_config_.GetAddress(),
		server_data_->server_config_.GetSSLPort());
    auto server_socket_ssl = std::make_shared<TCP::Socket>(true);
    server_socket_ssl->Listen(address_info_ssl);
    tcp_io_controller_.AddSocket(server_socket_ssl);
#endif

    logger.Log(LogLevel::INFO, "Server started!");
};

auto IRCServer::RunOnce() -> void
{	
	time_.tv_sec = time(NULL);

    tcp_io_controller_.RunOnce();

    tcp_io_controller_.AcceptNewConnections(
        [this](std::shared_ptr<TCP::ISocket> socket)
        {
            auto io_handler = std::make_unique<TCPIOHandler>(socket);
            auto client = std::make_unique<Client>(std::move(io_handler));

			client->SetPingTime(GetCurrentSecond() + server_data_->server_config_.GetPingTime());
            server_data_->client_database_.AddClient(std::move(client));
            logger.Log(LogLevel::DEBUG, "New client on FD: %d", socket->GetFD());
        });

    server_data_->client_database_.PollClients(
        [this](IClient* client, std::string raw_message)
        {
			// Reset ping time if we get activity from a client.
			client->SetPingTime(GetCurrentSecond() + server_data_->server_config_.GetPingTime());

            try {
                auto parsed_message = IRC::Parser::RunParser<IRC::RawMessage>(IRC::ParseRawMessage, raw_message);
                auto message = Message(client->GetUUID(), parsed_message);

                logger.Log(LogLevel::DEBUG, "Received: %s", raw_message.c_str());
                if (!message_dispatcher_->Dispatch(message))
                {
                    auto error_reply = GetErrorMessage(server_data_->server_config_.GetName(), ERR_UNKNOWNCOMMAND, message.GetCommand());
                    client->Push(error_reply);
                }

            } catch (ParseException &e) {
                logger.Log(LogLevel::WARNING, "Failed to parse: %s ", raw_message.c_str());
                client->Push("ERROR :Could not parse message");
            }
        });

	this->PingClients();
    server_data_->client_database_.SendAll();
}

auto IRCServer::CreateNewConnection(std::string &ip, std::string &port) -> std::optional<IClient*>
{
    logger.Log(LogLevel::DEBUG, "Attempting to create a new client on %s", ip.c_str());

    TCP::AddressInfo address_info(ip, port);

    auto server_socket = std::make_shared<TCP::Socket>(false);
    try {
        server_socket->Connect(address_info);
        logger.Log(LogLevel::DEBUG, "Connected new client");
    }
    catch (TCP::Socket::Error &ex){
        logger.Log(LogLevel::WARNING, "Failed to connect new client on %s on port %s", ip.c_str(), port.c_str());
        logger.Log(LogLevel::DEBUG, "Socket error: %s", ex.what());
        return std::nullopt;
    }
    tcp_io_controller_.AddSocket(server_socket);

    auto io_handler = std::make_unique<TCPIOHandler>(server_socket);
    auto client = std::make_unique<Client>(std::move(io_handler));
    return std::optional<IClient*>(server_data_->client_database_.AddClient(std::move(client)));
}

auto IRCServer::PingClients() -> void
{
	this->server_data_->client_database_.DoForEachClient(
		[&](IClient* client)
		{
			if (!client->ShouldPing(GetCurrentSecond()))
			{
				return ;
			}
			
			if (!client->RespondedToLastPing())
			{
				logger.Log(LogLevel::WARNING, ("Client did not respond to ping " + client->GetNickname()).c_str());
				server_data_->client_database_.DisconnectClient(client->GetUUID(),
					std::make_optional<std::string>("PING timeout"));
				return ;
			}

			client->SetRespondedToLastPing(false);
			client->SetPingTime(GetCurrentSecond() + server_data_->server_config_.GetPingTime());

			client->Push("PING " + server_data_->server_config_.GetName());
		}, std::nullopt);
}
