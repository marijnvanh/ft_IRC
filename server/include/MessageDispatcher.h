#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

#include <unordered_map>
#include <memory>

#include "MessageHandlers/ICommandHandler.h"
#include "IMessageDispatcher.h"
#include "ServerData.h"

using MessageHandler = std::function<void(ServerData*, Message)>;

class MessageDispatcher : public IMessageDispatcher
{
    public:
    MessageDispatcher(ServerData *server_data);
    ~MessageDispatcher();

    auto Dispatch(Message message) -> void override;

    private:
    ServerData* server_data_;
    std::unordered_map<std::string, MessageHandler> handlers_;
    std::unordered_map<std::string, std::unique_ptr<ICommandHandler>> command_handlers_;
};

#endif