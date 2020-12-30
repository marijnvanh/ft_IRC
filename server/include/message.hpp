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
**      <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
**
**      <prefix>   ::= <servername> | <nick> [ '!' <user> ] [ '@' <host> ]
**      <command>  ::= <letter> { <letter> } | <number> <number> <number>
**      <SPACE>    ::= ' ' { ' ' }
**      <params>   ::= <SPACE> [ ':' <trailing> | <middle> <params> ]
**
**      <middle>   ::= <Any *non-empty* sequence of octets not including SPACE
**      or NUL or CR or LF, the first of which may not be ':'>
**      <trailing> ::= <Any, possibly *empty*, sequence of octets not including
**                      NUL or CR or LF>
**
**      <crlf>     ::= CR LF
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

    struct RawUserPrefix {
        std::string nickname;
        std::string username;
        Hostname hostname;
    };

    struct RawPrefix {
        SenderType tag;
        union {
            std::string server_name;
            RawUserPrefix user_prefix;
        } value;
    };

    struct RawMessage {
        RawPrefix prefix;
        RawCommand command;
    };



    using namespace parser;

    /* <message>  ::= [':' < prefix > <SPACE>]<command><params><crlf> */
    /* <prefix>   ::= <servername> | <nick>['!' < user > ]['@' < host > ] */

    auto parseCommandId(CharStream& s) -> std::string;
    auto parseParams(CharStream& s) -> std::vector<std::string>;
    auto parseMiddle(CharStream& s) -> std::string;
    auto parseTrailing(CharStream& s) -> std::string;

    auto parseHostname(CharStream& s) -> Hostname;
    auto parseNickname(CharStream& s) -> std::string;
} // namespace ft_irc
#endif // MESSAGE_HPP
