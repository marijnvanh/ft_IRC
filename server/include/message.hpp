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
*/

namespace ft_irc {
    enum SenderType { IsClient, IsServer };

    struct RawCommand {
        std::string name;
        std::vector<std::string> parameters;
    };

    struct RawUserPrefix {
        std::string nickname;
        std::string username;
        std::string hostname;
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

/* <command>  ::= <letter>{<letter>} | <number><number><number> */
auto parseCommandId(CharStream& s) -> std::string;

/* <SPACE>    ::= ' ' {' '} */
/* This is just parseWhitespace */

/* <params>   ::= <SPACE>[':' < trailing > | <middle><params>] */
auto parseParams(CharStream& s) -> std::vector<std::string>;


/* <middle>   ::= <Any * non - empty * sequence of octets not including SPACE *
 * * or NUL or CR or LF, the first of which may not be ':' > */
/* <trailing> ::= < Any, possibly *empty *, sequence of octets not including *
 * *NUL or CR or LF > */


} // namespace ft_irc
#endif // MESSAGE_HPP
