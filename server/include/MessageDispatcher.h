#ifndef MESSAGE_DISPATCHER_HPP
#define MESSAGE_DISPATCHER_HPP

#include <unordered_map>
#include <memory>

#include "MessageHandlers/ICommandHandler.h"
#include "IMessageDispatcher.h"
#include "ServerData.h"

class MessageDispatcher : public IMessageDispatcher
{
    public:
    MessageDispatcher(ServerData *server_data);
    ~MessageDispatcher();

    auto Dispatch(Message message) -> void override;

    private:
    std::unordered_map<std::string, std::unique_ptr<ICommandHandler>> command_handlers_;
};

#endif