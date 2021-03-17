#ifndef JOIN_HANDLER_HPP
#define JOIN_HANDLER_HPP

#include "IMessage.h"
#include "IClientDatabase.h"
#include "IChannelDatabase.h"

auto JOINHandler(IClientDatabase *client_database,
	IChannelDatabase *channel_database, IMessage &message) -> void;

#endif
