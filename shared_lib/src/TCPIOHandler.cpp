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

auto SpliceToCRLF(std::string &buf) -> std::optional<std::string> {
    // FIXME: this performs pretty badly, but the logic should be as simple as this function exposes.
    auto f = buf.find("\r\n");
    if (f == std::string::npos) {
        return std::nullopt;
    } else if (f >= 512) {
        // there's too many bytes here for a valid IRC message, try find after CRLF
        auto ret = buf.substr(0, f);
        buf = buf.substr(f + 2);
        return SpliceToCRLF(buf);
    } else {
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
    catch (TCP::Socket::WouldBlock &ex)
    {
        return std::nullopt;
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
