#include "Message.h"
#include <memory>

TCP::Message::Message(std::shared_ptr<const Socket>socket, std::shared_ptr<std::string> data) :
                socket_(socket), data_(data), retries_(0)
{};

TCP::Message::Message(std::shared_ptr<const Socket>socket, std::string &&data) :
                socket_(socket),
                data_(std::make_shared<std::string>(data)),
                retries_(0)
{};

TCP::Message::~Message()
{}

/**
 * @brief Increase retry count after a try to send
 * 
 */
auto TCP::Message::Retry() -> void
{
    retries_++;
};
            
auto TCP::Message::GetSocket() const -> std::shared_ptr<const Socket>
{
    return socket_;
};

auto TCP::Message::GetData() -> std::shared_ptr<std::string>
{
    return data_;
};

auto TCP::Message::GetRetries() const -> int
{
    return retries_;
};

auto TCP::Message::GetFD() const -> int
{
    return socket_->GetFD();
};

auto TCP::operator<<(std::ostream& os, const TCP::Message& message) -> std::ostream &
{
    os << "FD: " << message.GetFD() << " Retries: " << message.GetRetries()
    << " Data: " << *message.data_ << std::endl;
    return os;
}