#ifndef PARSER_HPP
#define PARSER_HPP

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

            auto debug_stream() const -> void;

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

        class EOFException : public ParseException {
        public:
          EOFException(size_t location)
              : ParseException(location, true,
                               "Unexpected character. Expecting end of file.") {}

          ~EOFException() = default;
        };

        template<typename T>
        using Parser = std::function<T(CharStream& s)>;

        template<typename T>
        auto run(Parser<T> p, const std::string s) -> T {
            CharStream cs = CharStream::from_string(s);
            return p(cs);
        }

        template<typename T>
        auto attempt(Parser<T> fun, CharStream& s) -> T {
            CharStream ss = s;
            try {
                auto ret = fun(ss);
                s = ss;
                return ret;
            } catch (ParseException& e) {
                throw e;
            }
        }

        template<typename T>
        auto maybe(Parser<T> fun, CharStream& s)
            ->std::optional<T> {
            CharStream s2 = s;
            try {
                auto ret = fun(s);
                s = s2;
                return std::optional<T>{ret};
            } catch (ParseException& e) {
                return std::nullopt;
            }
          }

        auto replicate(Parser<char> fun, int n, CharStream &s)
            -> std::string;

        template<typename T>
        auto some(Parser<T> fun, CharStream &s)
            -> std::vector<T> {
            std::vector<T> accum;

            accum += fun(s);
            for (;;) {
                try {
                    accum += fun(s);
                } catch(ParseException& e) {
                    if (e.recoverable)
                        return accum;
                    else
                        throw e;
                }
            }
        }

        auto eof(CharStream& s) -> void;
        auto satisfy(std::function<bool(char)> predicate, CharStream& s) -> char;
        auto oneOf(std::string options, CharStream& s) -> char;
        auto consumeWhile(std::function<bool(char)> predicate, CharStream& s) -> std::string;
        auto consumeWhile1(std::function<bool(char)> predicate, CharStream &s)

            -> std::string;
        auto some(std::function<char(CharStream &s)> fun, CharStream& s)
            -> std::string;

        auto parseAlpha(CharStream& s) -> char;
        auto parseDigit(CharStream& s) -> char;
        auto parseSymbol(char c, CharStream& s) -> char;
        auto parseWhitespace(CharStream& s) -> void;
        auto parseWord(CharStream& s) -> std::string;
        auto parseString(std::string expected, CharStream& s) -> std::string;
    }
}

#endif // PARSER_HPP
