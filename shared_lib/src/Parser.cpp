#include "Parser.hpp"
#include <cctype>

using namespace ft_irc::parser;

auto ParseException::Explain(const CharStream& stream) const -> void {
  std::cout << "Parse Exception: "
            << what()
            << ", at "
            << location
            << ". Remaning: '"
            << stream.Remaining()
            << "'."
            << std::endl;
}

auto CharStream::ConsumeMany(const size_t count) -> std::string {
  if (read_head_ + count - 1 >= (*stream_).size())
    throw StreamEndException(read_head_);
  read_head_ += count;
  return (*stream_).substr(read_head_, read_head_ + count);
}

auto CharStream::Consume() -> char {
  if (read_head_ >= (*stream_).size())
    throw StreamEndException(read_head_);
  read_head_++;
  return (*stream_)[read_head_ - 1];
}

auto CharStream::Peek() const -> char {
  if (read_head_ >= (*stream_).size())
    throw StreamEndException(read_head_);
  return (*stream_)[read_head_];
}

auto CharStream::Remaining() const -> std::string {
  return (*stream_)
    .substr(read_head_, (*stream_).size() - read_head_);
}

auto CharStream::Location() const -> size_t {
  return read_head_;
}

auto CharStream::DebugStream() const -> void {
  std::cout << "Debugging stream at " << read_head_ << "." << std::endl;
  std::cout << "Remaining text: <<" << Remaining() << ">>" << std::endl;
}

auto ft_irc::parser::Replicate(std::function<char(CharStream &s)> fun, int n, CharStream &s) -> std::string {
  std::string accum;

  for (auto i = 0; i < n; i++) {
    try {
      accum += fun(s);
    } catch(ParseException& e) {
      throw e;
    }
  }
  return accum;
}

auto ft_irc::parser::eof(CharStream &s) -> void {
  if (s.Remaining().size() > 0)
    throw EOFException(s.Location());
  return;
}

auto ft_irc::parser::Satisfy(std::function<bool(char)> predicate, CharStream& s) -> char {
  const auto peeked = s.Peek();
  if (predicate(peeked))
    return s.Consume();
  else
    throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::OneOf(std::string options, CharStream& s) -> char {
  const auto peeked = s.Peek();
  if (options.find(peeked) == std::string::npos)
    throw MatchFailureException(s.Location(), peeked);
  return s.Consume();
}

auto ft_irc::parser::ConsumeWhile(std::function<bool(char)> predicate, CharStream &s)
  -> std::string {
  std::string accum;
  try {
    for (;;) {
      const auto peeked = s.Peek();
      if (!predicate(peeked))
        return accum;
      accum += s.Consume();
    }
  } catch(ParseException& e) {
    if (e.recoverable)
      return accum;
    else
      throw e;
  }
}

auto ft_irc::parser::ConsumeWhile1(std::function<bool(char)> predicate, CharStream &s)
  -> std::string {
  std::string accum;
  accum += Satisfy(predicate, s);
  accum += ConsumeWhile(predicate, s);
  return accum;
}

auto ft_irc::parser::ParseAlpha(CharStream &s) -> char {
  char peeked = s.Peek();
  if ((peeked >= 'a' && peeked <= 'z') ||
      (peeked >= 'A' && peeked <= 'Z'))
    return s.Consume();
  else
    throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::ParseDigit(CharStream &s) -> char {
  char peeked = s.Peek();
  if (peeked >= '0' && peeked <= '9')
    return s.Consume();
  else
    throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::ParseSymbol(char c, CharStream &s) -> char {
  char peeked = s.Peek();
  if (peeked == c)
    return s.Consume();
  else
    throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::ParseWhitespace(CharStream &s) -> void {
  ConsumeWhile([](char c) { return c == ' '; }, s);
}

auto ft_irc::parser::ParseWord(CharStream &s) -> std::string {
  return ConsumeWhile1([](char c){ return std::isalpha(c); }, s);
}

auto ft_irc::parser::ParseString(std::string expected, CharStream& s) -> std::string {
  for (size_t i = 0; i < expected.size(); i++) {
    ParseSymbol(expected[i], s);
  }

  return expected;
}
