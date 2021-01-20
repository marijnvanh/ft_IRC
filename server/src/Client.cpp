#include "Client.h"
#include <iostream>

Client::Client(ft_irc::IIOHandler &io_handler) : io_handler_(io_handler)
{}

Client::~Client()
{}

auto Client::Push(std::shared_ptr<std::string> irc_message) -> void
{
    outgoing_msg_queue_.push(irc_message);
}

auto Client::SendAll() -> void
{
    while (outgoing_msg_queue_.empty() == false)
    {
        auto irc_message = outgoing_msg_queue_.front();
        try {
            io_handler_.Send(*irc_message);
        }
        catch (ft_irc::IIOHandler::FailedToSend &ex)
        {
            std::cerr << "Client with UUID: " << UUID_;
            std::cerr << ", failed to send: " << ex.what() << std::endl;
            break ;
        }
        catch (ft_irc::IIOHandler::Closed &ex)
        {
            //TODO Handle closed
            break ;
        }
        outgoing_msg_queue_.pop();
    }
}

auto Client::Receive() -> std::optional<std::string>
{
    try {
        return std::optional<std::string>{io_handler_.Receive()};
    }
    catch (ft_irc::IIOHandler::FailedToReceive &ex)
    {
        std::cerr << "Client with UUID: " << UUID_;
        std::cerr << ", failed to Receive: " << ex.what() << std::endl;
        return std::nullopt;
    }
    catch (ft_irc::IIOHandler::Closed &ex)
    {
        //TODO Handle closed
        return std::nullopt;
    }
}
