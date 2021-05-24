#include "MessageDispatcher.h"
#include "MessageHandlers/PINGHandler.h"
#include "MessageHandlers/PONGHandler.h"
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
#include "MessageHandlers/SQUITHandler.h"
#include "MessageHandlers/ERRORHandler.h"
#include "MessageHandlers/NUMERICHandler.h"
#include "MessageHandlers/CONNECTHandler.h"
#include "MessageHandlers/OPERHandler.h"
#include "MessageHandlers/NOTICEHandler.h"
#include "MessageHandlers/TOPICHandler.h"
#include "MessageHandlers/NJOINHandler.h"

MessageDispatcher::MessageDispatcher(ServerData* server_data, IRCServer *irc_server) :
    logger("MD")
{
    command_handlers_.insert(std::make_pair("USER",
        std::make_unique<USERHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("QUIT",
        std::make_unique<QUITHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("KILL",
        std::make_unique<KILLHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("PART",
        std::make_unique<PARTHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("JOIN",
        std::make_unique<JOINHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("PING",
        std::make_unique<PINGHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("PONG",
        std::make_unique<PONGHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("PASS",
        std::make_unique<PASSHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("PRIVMSG",
        std::make_unique<PRIVMSGHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("NAMES",
        std::make_unique<NAMESHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("MODE",
        std::make_unique<MODEHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
	);
    command_handlers_.insert(std::make_pair("SERVER",
        std::make_unique<SERVERHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("NICK",
        std::make_unique<NICKHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("SQUIT",
        std::make_unique<SQUITHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("ERROR",
        std::make_unique<ERRORHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("NUMERIC",
        std::make_unique<NUMERICHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("CONNECT",
        std::make_unique<CONNECTHandler>(&server_data->server_config_, &server_data->client_database_, irc_server))
    );
    command_handlers_.insert(std::make_pair("OPER",
        std::make_unique<OPERHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("NOTICE",
        std::make_unique<NOTICEHandler>(&server_data->server_config_, &server_data->client_database_))
    );
    command_handlers_.insert(std::make_pair("TOPIC",
        std::make_unique<TOPICHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
    );
    command_handlers_.insert(std::make_pair("NJOIN",
        std::make_unique<NJOINHandler>(&server_data->server_config_, &server_data->client_database_, &server_data->channel_database_))
    );
}

MessageDispatcher::~MessageDispatcher()
{}

static auto CheckIfNumeric(const std::string &numeric) -> bool
{
    if (numeric.size() == 3 &&
        std::isdigit(numeric[0]) &&
        std::isdigit(numeric[1]) &&
        std::isdigit(numeric[2]))
    {
        return true;
    }
    return false;
}

auto MessageDispatcher::Dispatch(Message message) -> bool
{
    auto command_handler = command_handlers_.find(message.GetCommand());

    if (command_handler != command_handlers_.end())
    {
        logger.Log(LogLevel::DEBUG, "Handling command: %s", message.GetCommand().c_str());
        command_handler->second->Handle(message);
        return true;
    }
    else if (CheckIfNumeric(message.GetCommand()))
    {
        logger.Log(LogLevel::DEBUG, "Handling numeric: %s", message.GetCommand().c_str());
        auto numeric_handler = command_handlers_.find("NUMERIC");
        numeric_handler->second->Handle(message);
        return true;
    }
    else
    {
        logger.Log(LogLevel::WARNING, "Received invalid command: %s", message.GetCommand().c_str());
        return false;
    }
}
