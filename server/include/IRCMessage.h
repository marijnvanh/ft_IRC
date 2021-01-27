#ifndef __IRC__MESSAGE_H__
#define __IRC__MESSAGE_H__

#include <string>
#include "RawMessage.h" //TODO How to namesapce header files

struct IRCMessage
{
    int UUID_;
    ft_irc::RawMessage message_;

    IRCMessage(int UUID, ft_irc::RawMessage message) : UUID_(UUID), message_(message) {}
};

#endif