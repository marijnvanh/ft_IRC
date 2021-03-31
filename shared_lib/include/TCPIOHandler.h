#ifndef __TCP_IOHANDLER_H__
#define __TCP_IOHANDLER_H__

#include <string>
#include <memory>

#include "IIOHandler.h"
#include "ISocket.h"

namespace IRC
{
    class TCPIOHandler : public IRC::IIOHandler
    {
    private:
        std::shared_ptr<TCP::ISocket> socket_;
        std::string splice_buffer_;

    public:
        TCPIOHandler(std::shared_ptr<TCP::ISocket> socket);
        ~TCPIOHandler();

        auto Send(const std::string data) -> void;
        auto Receive() -> std::optional<std::string>;
    };

} // namespace IRC::TCP

#endif
