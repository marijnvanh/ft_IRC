#include "Client.h"
#include <iostream>

#define DELIMITER "\r\n"

Client::Client(std::unique_ptr<IRC::IIOHandler> io_handler) :
    io_handler_(std::move(io_handler)),
    uuid_(IRC::UUIDGenerator::GetInstance().Generate()),
    logger("Client")

{
    logger.Log(LogLevel::INFO, "Client connected");
}

Client::~Client()
{
    logger.Log(LogLevel::INFO, "Client disconnected");
}

auto Client::Push(std::string irc_message) -> void
{
    outgoing_msg_queue_.push(irc_message + DELIMITER);
}

auto Client::SendAll() -> void
{
    while (outgoing_msg_queue_.empty() == false)
    {
        auto irc_message = outgoing_msg_queue_.front();
        try {
            io_handler_->Send(irc_message);
        }
        catch (IRC::IIOHandler::FailedToSend &ex)
        {
            // std::cerr << "Client with UUID: " << UUID_;
            std::cerr << ", failed to send: " << ex.what() << std::endl;
            break ;
        }
        catch (IRC::IIOHandler::Closed &ex)
        {
            throw IClient::Disconnected(ex.what());
        }
        outgoing_msg_queue_.pop();
    }
}

auto Client::Receive() -> std::optional<std::string>
{
    try {
        return std::optional<std::string>{io_handler_->Receive()};
    }
    catch (IRC::IIOHandler::Closed &ex)
    {
        throw IClient::Disconnected(ex.what());
    }
}
