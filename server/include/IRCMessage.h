#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include <string>
#include "RawMessage.h" //TODO How to namesapce header files

struct IRCMessage
{
    int uuid_;
    IRC::RawMessage message_;

    IRCMessage(int uuid, IRC::RawMessage message) : uuid_(uuid), message_(message) {}
};

#endif