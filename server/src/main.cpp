#include <iostream>
#include <vector>
#include <memory>
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


auto parseAlpha(CharStream &s) -> char {
    char peeked = s.peek();
    if ((peeked >= 'a' && peeked <= 'z') ||
        (peeked >= 'A' && peeked <= 'Z'))
        return s.consume();
    else
        throw MatchFailureException(s.location(), peeked);
}

auto parseSymbol(char c, CharStream &s) -> char {
    char peeked = s.peek();
    if (peeked == c)
        return s.consume();
    else
        throw MatchFailureException(s.location(), peeked);
}

auto whitespace(CharStream &s) -> void {
    try {
        for (;;) {
            char peeked = s.peek();
            if (peeked == ' ')
                s.consume();
            else
                return;
        }
    } catch(ParseException& e) {
        if (e.recoverable)
            return;
        else
            throw e;
    }
}

auto parseWord(CharStream &s) -> std::string {
    std::string accum("");

    accum += parseAlpha(s);
    for (;;) {
        try {
            accum += parseAlpha(s);
        } catch(ParseException& e) {
            if (e.recoverable)
                return accum;
            else
                throw e;
        }
    }
}

auto parseString(std::string expected, CharStream &s) -> std::string {
    for (size_t i = 0; i < expected.size(); i++) {
        parseSymbol(expected[i], s);
    }

    return expected;
}

auto greeting(CharStream &s) -> std::string {
    try {
        CharStream s2 = s;
        std::string ret = parseString("hello", s2);
        s = s2;
        return ret;
    } catch (const ParseException& e) {
        return parseString("hi", s);
    }
}

int main()
{
    CharStream s1 = CharStream::from_string("hi, world");

    try {
        std::string a = greeting(s1);
        whitespace(s1);
        parseSymbol(',', s1);
        whitespace(s1);
        std::string b = parseWord(s1);
        std::cout << "Got two words: '" << a << "', '" << b << "'" << std::endl;
    } catch (ParseException& e) {
        e.explain(s1);
    }
}
