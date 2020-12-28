#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "Socket.h"

namespace TCP
{
    struct Message
    {
        private:
            const Socket &socket_;
            std::string data_;

        public:
            Message(const Socket &socket, std::string &&data) :
                socket_(socket), data_(data) {};

            std::string GetData()
            {
                return data_;
            };

            const Socket &GetSocket()
            {
                return socket_;
            };
    };
}

#endif