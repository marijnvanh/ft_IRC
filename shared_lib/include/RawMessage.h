#ifndef RAW_MESSAGE_HPP
#define RAW_MESSAGE_HPP

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "Parser.h"

/*
**
** Pseudo BNF specification
**
**      <message>    ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
**
**      <prefix>     ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
**      <command>    ::= <letter> { <letter> } | <number> <number> <number>
**      <SPACE>      ::= ' ' { ' ' }
**      <params>     ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
**
**      <middle>     ::= <Any *non-empty* sequence of octets not including SPACE
**      or NUL or CR or LF, the first of which may not be ':'>
**      <trailing>   ::= <Any, possibly *empty*, sequence of octets not including
**                      NUL or CR or LF>
**
**      <crlf>       ::= CR LF
**
**      <target>     ::= <to> [ "," <target> ]
**      <to>         ::= <channel> | <user> '@' <servername> | <nick> | <mask>
**      <channel>    ::= ('#' | '&') <chstring>
**      <servername> ::= <host>
**      <host>       ::= see RFC 952 [DNS:4] for details on allowed hostnames
**
**      <nick>       ::= <letter> { <letter> | <number> | <special> }
**      <mask>       ::= ('#' | '$') <chstring>
**      <chstring>   ::= <any 8bit code except SPACE, BELL, NUL, CR, LF and
** comma (',')>
**
**      <user>       ::= <nonwhite> { <nonwhite> }
**      <letter>     ::= 'a' ... 'z' | 'A' ... 'Z'
**      <number>     ::= '0' ... '9'
**      <special>    ::= '-' | '[' | ']' | '\' | '`' | '^' | '{' | '}'
**
*/
namespace IRC {

    // Domain newtypes
    struct Hostname {
        std::string value;

        Hostname(std::string hostname) : value(hostname) {}
    };

    enum SenderType { IsClient, IsServer };

    struct RawCommand {
        std::string name;
        std::vector<std::string> parameters;
    };

    struct RawPrefix {
        //TODO FIXME: currently this does not distinguish between server / client message prefix
        // even though the RFC explicitly states the difference (hint: servername has only a *hostname*)
        // Parser is broken in the same way.
        std::string name;
        std::optional<std::string> username;
        std::optional<Hostname> hostname;

		bool is_server_origin;
    };

    struct RawMessage {
        std::optional<RawPrefix> prefix;
        RawCommand command;
    };

    using namespace IRC::Parser;

    auto ParseRawMessage(CharStream& s) -> RawMessage;
    auto ParsePrefix(CharStream& s) -> RawPrefix;

    auto ParseCommandId(CharStream& s) -> std::string;
    auto ParseParams(CharStream& s) -> std::vector<std::string>;
    auto ParseMiddle(CharStream& s) -> std::string;
    auto ParseTrailing(CharStream& s) -> std::string;

    auto ParseHostname(CharStream& s) -> Hostname;
    auto ParseNickname(CharStream& s) -> std::string;

	auto TryParseServername(CharStream& s) -> std::optional<Hostname>;
} // namespace IRC

#endif // RATW_MESSAGE_HPP
