#ifndef __I_IOHANDLER_H__
#define __I_IOHANDLER_H__

#include <string>
#include <stdexcept>

namespace ft_irc
{
    class IIOHandler
    {
        public:

        /**
         * @brief Send data
         * 
         * @exception IOFailedToSend (recoverable) IOClosed (non-recoverable)
         * @param data 
         */
        virtual auto Send(const std::string data) -> void = 0;

        /**
         * @brief Receive data
         * 
         * @exception IOFailedToSend (recoverable) IOClosed (non-recoverable)
         * @param data 
         */
        virtual auto Receive() -> std::string = 0;

        virtual ~IIOHandler() {};
        
        class FailedToSend : public std::runtime_error
        {
        public:
            FailedToSend(const char *msg) : std::runtime_error(msg) {}
        };

        class FailedToReceive : public std::runtime_error
        {
        public:
            FailedToReceive(const char *msg) : std::runtime_error(msg) {}
        };

        class Closed : public std::runtime_error
        {
        public:
            Closed(const char *msg) : std::runtime_error(msg) {}
        };
    };
}

#endif
