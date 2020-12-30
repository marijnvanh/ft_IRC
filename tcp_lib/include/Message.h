#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "Socket.h"
#include <memory>

namespace TCP
{
    struct Message
    {
        private:
            const std::shared_ptr<const Socket> socket_;
            std::string data_;
            int retries_;

        public:
            Message(const std::shared_ptr<const Socket>socket, std::string &&data) :
                socket_(socket), data_(data), retries_(0) {};

            std::string &GetData()
            {
                return data_;
            };

            const std::shared_ptr<const Socket> GetSocket() const
            {
                return socket_;
            };

            void Retry() { retries_++; };
            int GetRetries() { return retries_; };
            int GetFD() const { return socket_->GetFD(); };
    };
}

#endif