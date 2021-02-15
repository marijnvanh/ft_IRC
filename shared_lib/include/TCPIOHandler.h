#ifndef __TCP_IOHANDLER_H__
#define __TCP_IOHANDLER_H__

#include <string>
#include <memory>

#include "IIOHandler.h"
#include "Socket.h"

namespace IRC
{
    class TCPIOHandler : public IRC::IIOHandler
    {
    private:
        std::shared_ptr<TCP::Socket> socket_; //TODO Make mutex

    public:
        TCPIOHandler(std::shared_ptr<TCP::Socket> socket);
        ~TCPIOHandler();

        auto Send(const std::string data) -> void;
        auto Receive() -> std::optional<std::string>;
    };

} // namespace IRC::TCP

#endif
