#include "Message.h"
#include <memory>

TCP::Message::Message(std::shared_ptr<const Socket>socket, std::string &&data) :
                socket_(socket), data_(data), retries_(0)
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

auto TCP::Message::GetData() const -> const std::string&
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

TCP::Message::Message (const Message& other)
{
    socket_ = other.GetSocket();
    data_ = other.GetData();
    retries_ = other.GetRetries();
}

auto TCP::Message::operator =(const Message& other) -> Message &
{
    socket_ = other.GetSocket();
    data_ = other.GetData();
    retries_ = other.GetRetries();
    return *this;
}

TCP::Message::Message (const Message&& other)
{
    socket_ = other.GetSocket();
    data_ = other.GetData(); //TODO how do we use move here since it's const?
    retries_ = other.GetRetries();
}

auto TCP::Message::operator =(const Message&& other) -> Message &
{
    socket_ = other.GetSocket();
    data_ = other.GetData(); //TODO how do we use move here since it's const?
    retries_ = other.GetRetries();
    return *this;
}

auto operator<<(std::ostream& os, const TCP::Message& message) -> std::ostream &
{
    os << "FD: " << message.GetFD() << " Retries: " << message.GetRetries()
    << " Data: " << message.GetData() << std::endl;
    return os;
}