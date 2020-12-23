#include "parser.hpp"

using namespace ft_irc::parser;

auto ParseException::explain(const CharStream& stream) const -> void {
    std::cout << "Parse Exception: "
              << what()
              << ", at "
              << location
              << ". Remaning: '"
              << stream.remaining()
              << "'."
              << std::endl;
}

auto CharStream::consume_many(const size_t count) -> std::string {
    if (read_head_ + count - 1 >= (*stream_).size())
        throw StreamEndException(read_head_);
    read_head_ += count;
    return (*stream_).substr(read_head_, read_head_ + count);
}

auto CharStream::consume() -> char {
    if (read_head_ >= (*stream_).size())
        throw StreamEndException(read_head_);
    read_head_++;
    return (*stream_)[read_head_ - 1];
}

auto CharStream::peek() const -> char {
    if (read_head_ >= (*stream_).size())
        throw StreamEndException(read_head_);
    return (*stream_)[read_head_];
}

auto CharStream::remaining() const -> std::string {
    return (*stream_)
        .substr(read_head_, (*stream_).size() - read_head_);
}

auto CharStream::location() const -> size_t {
    return read_head_;
}

auto ft_irc::parser::parseAlpha(CharStream &s) -> char {
    char peeked = s.peek();
    if ((peeked >= 'a' && peeked <= 'z') ||
        (peeked >= 'A' && peeked <= 'Z'))
        return s.consume();
    else
        throw MatchFailureException(s.location(), peeked);
}

auto ft_irc::parser::parseSymbol(char c, CharStream &s) -> char {
    char peeked = s.peek();
    if (peeked == c)
        return s.consume();
    else
        throw MatchFailureException(s.location(), peeked);
}

auto ft_irc::parser::parseWhitespace(CharStream &s) -> void {
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

auto ft_irc::parser::parseWord(CharStream &s) -> std::string {
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

auto ft_irc::parser::parseString(std::string expected, CharStream& s) -> std::string {
    for (size_t i = 0; i < expected.size(); i++) {
        parseSymbol(expected[i], s);
    }

    return expected;
}
