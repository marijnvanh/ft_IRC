#include "MessageDispatcher.h"
#include "MessageHandlers/PINGHandler.h"
#include "MessageHandlers/PASSHandler.h"
#include "MessageHandlers/NICKHandler.h"
#include "MessageHandlers/USERHandler.h"
#include "MessageHandlers/KILLHandler.h"
#include "MessageHandlers/QUITHandler.h"
#include "MessageHandlers/PRIVMSGHandler.h"
#include "MessageHandlers/JOINHandler.h"
#include "MessageHandlers/NAMESHandler.h"
#include "MessageHandlers/MODEHandler.h"

MessageDispatcher::MessageDispatcher(ServerData* server_data) 
    : server_data_(server_data)
{
    handlers_.insert(std::make_pair("NICK", [](auto server_data, auto message) {
            NICKHandler(&server_data->server_config_, &server_data->client_database_, message);
        }));
    handlers_.insert(std::make_pair("USER", [](auto server_data, auto message) {
            USERHandler(&server_data->server_config_, &server_data->client_database_, message);
        }));
    handlers_.insert(std::make_pair("KILL", [](auto server_data, auto message) {
            KILLHandler(&server_data->client_database_, message);
        }));
    handlers_.insert(std::make_pair("QUIT", [](auto server_data, auto message) {
            QUITHandler(&server_data->client_database_, message);
        }));

    command_handlers_.insert(std::make_pair("JOIN",
        std::make_unique<JOINHandler>(&server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("PING",
        std::make_unique<PINGHandler>(&server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("PASS",
        std::make_unique<PASSHandler>(&server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("PRIVMSG",
        std::make_unique<PRIVMSGHandler>(&server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("NAMES",
        std::make_unique<NAMESHandler>(&server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("MODE",
        std::make_unique<MODEHandler>(
			&server_data->server_config_,
			&server_data->client_database_,
			&server_data->channel_database_))
    );
    
}

MessageDispatcher::~MessageDispatcher()
{}

auto MessageDispatcher::Dispatch(Message message) -> void
{
    auto handler = handlers_.find(message.GetCommand());
    auto command_handler = command_handlers_.find(message.GetCommand());

    if (handler != handlers_.end())
        handler->second(server_data_, message);
    else if (command_handler != command_handlers_.end())
        command_handler->second->Handle(message);
    else    //TODO Handle invalid message
        std::cerr << "Received invalid message" << std::endl;
}
