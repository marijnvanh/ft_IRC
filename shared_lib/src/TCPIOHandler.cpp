#include "TCPIOHandler.h"
#include <iostream>

using namespace IRC;

TCPIOHandler::TCPIOHandler(std::shared_ptr<TCP::ISocket> socket) : socket_(socket)
{
}

TCPIOHandler::~TCPIOHandler()
{
    socket_->Close();
}

auto TCPIOHandler::Send(const std::string data) -> void
{
    try
    {
        socket_->Send(data);
    }
    catch (TCP::ISocket::WouldBlock &ex)
    {
        throw IIOHandler::FailedToSend(ex.what());
    }
    catch (TCP::ISocket::Closed &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
    catch (TCP::ISocket::Error &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
}

auto SpliceToCRLF(std::string &buf) -> std::optional<std::string> {
    auto f = buf.find("\r\n");
    if (f == std::string::npos) {
        return std::nullopt;
    }
    else if (f >= 512) {
        /* There's too many bytes here for a valid IRC message, try find after CRLF */
        buf = buf.substr(f + 2);
        return SpliceToCRLF(buf);
    }
    else {
        auto ret = buf.substr(0, f);
        buf = buf.substr(f + 2);
        return std::optional{ret};
    }
}

auto TCPIOHandler::Receive() -> std::optional<std::string>
{
    try
    {
        auto spliced = SpliceToCRLF(splice_buffer_);
        if (spliced) {
            return spliced;
        } else {
            auto socket_state = socket_->GetState();
            if (socket_state == TCP::SocketState::kReadyToRead) {
                splice_buffer_ += socket_->Recv();
                return SpliceToCRLF(splice_buffer_);
            } else if (socket_state != TCP::SocketState::kConnected)
                throw IIOHandler::Closed("Found socket in non connected state");
            else
                return std::nullopt;
        }
    }
    catch (TCP::ISocket::WouldBlock &ex)
    {
        return std::nullopt;
    }
    catch (TCP::ISocket::Closed &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
    catch (TCP::ISocket::Error &ex)
    {
        throw IIOHandler::Closed(ex.what());
    }
}
