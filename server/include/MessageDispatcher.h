#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

#include "IMessageDispatcher.h"
#include <unordered_map>
#include <ServerData.h>

using MessageHandler = std::function<void(std::shared_ptr<ServerData>, std::string)>;

class MessageDispatcher : public IMessageDispatcher
{
    public:
    MessageDispatcher(std::shared_ptr<ServerData> server_data);
    ~MessageDispatcher();

    auto Dispatch(std::string message) -> void override;

    private:
    std::shared_ptr<ServerData> server_data_;
    std::unordered_map<std::string, MessageHandler> handlers_;
};

#endif