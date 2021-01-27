#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include <string>
#include "RawMessage.h" //TODO How to namesapce header files

struct IRCMessage
{
    int UUID_;
    IRC::RawMessage message_;

    IRCMessage(int UUID, IRC::RawMessage message) : UUID_(UUID), message_(message) {}
};

#endif