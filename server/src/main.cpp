#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include "../include/parser.hpp"

/*
**
** Pseudo BNF specification
**
**      <message>  ::= [':' <prefix> <SPACE> ] <command> <params> <crlf>
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
}



using namespace ft_irc::parser;
auto greeting(CharStream& s) -> std::string {
    try {
        return attempt<std::string>([](CharStream& s) { return parseString("hello", s); }, s);
    } catch (const ParseException& e) {
        return parseString("hi", s);
    }
}

int main()
{
    CharStream s1 = CharStream::from_string("hello, world");

    try {
        std::string a = greeting(s1);
        parseWhitespace(s1);
        parseSymbol(',', s1);
        parseWhitespace(s1);
        std::string b = parseWord(s1);
        std::cout << "Got two words: '" << a << "', '" << b << "'" << std::endl;
    } catch (ParseException& e) {
        e.explain(s1);
    }
}
