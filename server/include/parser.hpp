#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

namespace ft_irc {
    namespace parser {
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

        template<typename T>
        auto attempt(std::function<T(CharStream& s)> fun, CharStream& s) -> T {
            CharStream s2 = s;
            try {
                auto ret = fun(s2);
                s = s2;
                return ret;
            } catch (ParseException& e) {
                throw e;
            }
        }

        template<typename T>
        auto maybe(std::function<T(CharStream& s)> fun, CharStream& s) -> std::optional<T> {
            CharStream s2 = s;
            try {
                auto ret = fun(s);
                s = s2;
                return std::optional<T>{ret};
            } catch (ParseException& e) {
                return std::nullopt;
            }
        }

        auto parseAlpha(CharStream &s) -> char;
        auto parseSymbol(char c, CharStream &s) -> char;
        auto parseWhitespace(CharStream &s) -> void;
        auto parseWord(CharStream &s) -> std::string;
        auto parseString(std::string expected, CharStream& s) -> std::string;
    }
}

#endif // CLIENT_HPP
