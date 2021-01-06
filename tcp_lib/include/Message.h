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
            std::shared_ptr<std::string> data_;
            int retries_;

        public:
            Message(std::shared_ptr<const Socket>socket, std::shared_ptr<std::string>data);
            Message(std::shared_ptr<const Socket>socket, std::string &&data);
            ~Message();

            auto Retry() -> void;
            
            auto GetSocket() const -> std::shared_ptr<const Socket>;
            auto GetData() -> std::shared_ptr<std::string>;
            auto GetRetries() const -> int;
            auto GetFD() const -> int;
    
            Message (const Message& other) = default;
            auto operator =(const Message& other) -> Message & = default;
            Message (Message&& other) = default;
            auto operator =(Message&& other) -> Message & = default;
        
            friend auto operator<<(std::ostream& os, const TCP::Message& message) -> std::ostream&;
    };
}


#endif