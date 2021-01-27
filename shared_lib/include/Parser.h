#ifndef PARSER_HPP
#define PARSER_HPP

#include <stdexcept>
#include <iostream>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

namespace IRC::Parser {
	class CharStream {
		size_t read_head_;
		std::shared_ptr<const std::string> stream_;
	public:
		auto ConsumeMany(const size_t count) -> std::string;
		auto Consume() -> char;
		auto Peek() const -> char;
		auto Remaining() const -> std::string;
		auto Location() const -> size_t;

		auto DebugStream() const -> void;

		static auto FromString(const std::string source_string) -> CharStream {
			return CharStream(std::make_shared<const std::string>(source_string));
		}

		static auto FromString(const char* source_string) -> CharStream {
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

		auto Explain(const CharStream& stream) const -> void;

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
	auto RunParser(Parser<T> p, const std::string s) -> T {
		CharStream cs = CharStream::FromString(s);
		return p(cs);
	}

	template<typename T>
	auto Attempt(Parser<T> fun, CharStream& s) -> T {
		CharStream s_snapshot = s;
		try {
			auto ret = fun(s_snapshot);
			s = s_snapshot;
			return ret;
		} catch (ParseException& e) {
			throw e;
		}
	}

	template<typename T>
	auto Maybe(Parser<T> fun, CharStream& s) -> std::optional<T> {
		CharStream s_snapshot = s;
		try {
			auto ret = fun(s_snapshot);
			s = s_snapshot;
			return std::optional<T>{ret};
		} catch (ParseException& e) {
			return std::nullopt;
		}
	}

	auto Replicate(Parser<char> fun, int n, CharStream& s) -> std::string;

	template<typename T>
	auto Some(Parser<T> fun, CharStream& s) -> std::vector<T> {
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

	auto Eof(CharStream& s) -> void;
	auto Satisfy(std::function<bool(char)> predicate, CharStream& s) -> char;
	auto OneOf(std::string options, CharStream& s) -> char;
	auto ConsumeWhile(std::function<bool(char)> predicate, CharStream& s) -> std::string;
	auto ConsumeWhile1(std::function<bool(char)> predicate, CharStream& s) -> std::string;

	auto ParseAlpha(CharStream& s) -> char;
	auto ParseDigit(CharStream& s) -> char;
	auto ParseSymbol(char c, CharStream& s) -> char;
	auto ParseWhitespace(CharStream& s) -> void;
	auto ParseWord(CharStream& s) -> std::string;
	auto ParseString(std::string expected, CharStream& s) -> std::string;

} // namespace IRC::Parser

#endif // PARSER_HPP
