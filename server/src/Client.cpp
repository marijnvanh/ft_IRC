#include "Client.h"
#include <iostream>

#define DELIMITER "\r\n"

Client::Client(std::unique_ptr<IRC::IIOHandler> io_handler) :
    io_handler_(std::move(io_handler)),
    uuid_(IRC::UUIDGenerator::GetInstance().Generate()),
    logger("Client")

{
    logger.Log(LogLevel::INFO, "Client connected");

	SetRespondedToLastPing(true);
}

Client::~Client()
{
    try {
        SendAll();
    }
    catch (IClient::Disconnected &ex)
    {
        (void)ex;
    }
    logger.Log(LogLevel::DEBUG, "Client object destroyed");
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
            logger.Log(LogLevel::WARNING, "Failed to send: %s", ex.what());
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
