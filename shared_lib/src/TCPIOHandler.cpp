#include "TCPIOHandler.h"
#include <iostream>

using namespace IRC;

TCPIOHandler::TCPIOHandler(std::shared_ptr<TCP::Socket> socket) : socket_(socket)
{
}

TCPIOHandler::~TCPIOHandler()
{
}

auto TCPIOHandler::Send(const std::string data) -> void
{
    try
    {
        socket_->Send(data);
    }
    catch (TCP::Socket::WouldBlock &ex)
    {
        throw IIOHandler::FailedToSend(ex.what());
    }
    catch (TCP::Socket::Closed &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
    catch (TCP::Socket::Error &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
}

auto TCPIOHandler::Receive() -> std::string //TODO change to optional
{
    try
    {
        return socket_->Recv();
    }
    catch (TCP::Socket::WouldBlock &ex)
    {
        throw IIOHandler::FailedToReceive(ex.what()); //TODO remove FailedToReceive and return nullopt
    }
    catch (TCP::Socket::Closed &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
    catch (TCP::Socket::Error &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
}
