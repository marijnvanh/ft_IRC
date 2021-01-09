#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../include/parser.hpp"

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
namespace ft_irc {

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
        // FIXME: currently this does not distinguish between server / client message prefix
        // even though the RFC explicitly states the difference (hint: servername has only a *hostname*)
        // Parser is broken in the same way.
        std::string name;
        std::optional<std::string> username;
        std::optional<Hostname> hostname;
    };

    struct RawMessage {
        std::optional<RawPrefix> prefix;
        RawCommand command;
    };

    using namespace parser;

    /* <message>  ::= [':' < prefix > <SPACE>]<command><params><crlf> */
    auto parseRawMessage(CharStream& s) -> RawMessage;
    auto parsePrefix(CharStream& s) -> RawPrefix;

    auto parseCommandId(CharStream& s) -> std::string;
    auto parseParams(CharStream& s) -> std::vector<std::string>;
    auto parseMiddle(CharStream& s) -> std::string;
    auto parseTrailing(CharStream& s) -> std::string;

    auto parseHostname(CharStream& s) -> Hostname;
    auto parseNickname(CharStream& s) -> std::string;
} // namespace ft_irc
#endif // MESSAGE_HPP
