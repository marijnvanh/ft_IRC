#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "Socket.h"
#include <memory>
#include <iostream>

namespace TCP
{
    struct Message
    {
        private:
            std::shared_ptr<const Socket> socket_;
            std::string data_; //TODO make shared
            int retries_;

        public:
            Message(std::shared_ptr<const Socket>socket, std::string &&data);
            ~Message();

            auto Retry() -> void;
            
            auto GetSocket() const -> std::shared_ptr<const Socket>;
            auto GetData() const -> const std::string &;
            auto GetRetries() const -> int;
            auto GetFD() const -> int;
    
            Message (const Message& other);
            auto operator =(const Message& other) -> Message &;
            Message (const Message&& other);
            auto operator =(const Message&& other) -> Message &;
    };
}

inline std::ostream& operator<<(std::ostream& os, const TCP::Message& message)
{
    os << "FD: " << message.GetFD() << " Retries: " << message.GetRetries()
    << " Data: " << message.GetData() << std::endl;
    return os;
}

#endif