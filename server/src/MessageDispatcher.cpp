#include "MessageDispatcher.h"
#include "MessageHandlers/PINGHandler.h"
#include "MessageHandlers/PASSHandler.h"
#include "MessageHandlers/NICKHandler.h"
#include "MessageHandlers/USERHandler.h"
#include "MessageHandlers/KILLHandler.h"
#include "MessageHandlers/QUITHandler.h"
#include "MessageHandlers/PRIVMSGHandler.h"
#include "MessageHandlers/JOINHandler.h"
#include "MessageHandlers/PARTHandler.h"
#include "MessageHandlers/NAMESHandler.h"
#include "MessageHandlers/MODEHandler.h"
#include "MessageHandlers/SERVERHandler.h"

MessageDispatcher::MessageDispatcher(ServerData* server_data) :
    logger("MD")
{
    command_handlers_.insert(std::make_pair("USER",
        std::make_unique<USERHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("QUIT",
        std::make_unique<QUITHandler>(&server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("KILL",
        std::make_unique<KILLHandler>(&server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("PART",
        std::make_unique<PARTHandler>(&server_data->client_database_, &server_data->channel_database_))
    );
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
        std::make_unique<MODEHandler>(&server_data->client_database_, &server_data->channel_database_))
	);
    command_handlers_.insert(std::make_pair("SERVER",
        std::make_unique<SERVERHandler>(&server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("NICK",
        std::make_unique<NICKHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    
}

MessageDispatcher::~MessageDispatcher()
{}

auto MessageDispatcher::Dispatch(Message message) -> void
{
    auto command_handler = command_handlers_.find(message.GetCommand());

    if (command_handler != command_handlers_.end())
        command_handler->second->Handle(message);
    else    //TODO Handle invalid message
        logger.Log(LogLevel::WARNING, "Received invalid command: %s", message.GetCommand().c_str());
}
