#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>

class CharStream {
    size_t read_head_;
    std::shared_ptr<const std::string> stream_;
public:
    auto consume_many(const size_t count) -> std::string;
    auto consume() -> char;
    auto peek() const -> char;
    auto remaining() const -> std::string;
    auto location() const -> size_t;

    static auto from_string(const std::string source_string) -> CharStream {
        return CharStream(std::make_shared<const std::string>(source_string));
    }

    static auto from_string(const char* source_string) -> CharStream {
        return CharStream(std::make_shared<const std::string>(source_string));
    }

    CharStream(std::shared_ptr<const std::string> source_string):
        read_head_(0), stream_(source_string) {}

    ~CharStream() = default;
};

class ParseException : public std::runtime_error {
public:
    size_t location;
    bool recoverable;

    auto explain(const CharStream& stream) const -> void;

    ParseException(size_t location, bool recoverable)
        : std::runtime_error("Generic parse exception"),
          location(location),
          recoverable(recoverable)
        {}

    ParseException(size_t location, bool recoverable,
                   const std::string &explanation)
        : std::runtime_error(explanation),
          location(location),
          recoverable(recoverable)
        {}

    ~ParseException() = default;
};

class StreamEndException : public ParseException {
public:
    StreamEndException(size_t location)
        : ParseException(location, true, "Stream ran out of characters to consume")
        {}

    ~StreamEndException() = default;
};

class MatchFailureException : public ParseException {
public:
    MatchFailureException(size_t location, char matched)
        : ParseException(location, true, "Unexpected char '" + std::string(1, matched) + "'") {}

    ~MatchFailureException() = default;
};

#endif // CLIENT_HPP
