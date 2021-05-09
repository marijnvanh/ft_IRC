#include "Parser.h"
#include <cctype>

using namespace IRC::Parser;

auto ParseException::Explain(const CharStream& stream) const -> void {
  std::cout << "Parse Exception: "
            << what()
            << ", at "
            << location
            << ". Tail: '"
            << stream.Tail()
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

auto CharStream::Remaining() const -> size_t {
  return stream_->size() - read_head_;
}

auto CharStream::Tail() const -> std::string {
  return (*stream_)
    .substr(read_head_, (*stream_).size() - read_head_);
}

auto CharStream::Location() const -> size_t {
  return read_head_;
}

auto CharStream::SetLocation(size_t location) -> void {
  read_head_ = location;
}

auto CharStream::DebugStream() const -> void {
  std::cout << "Debugging stream at " << read_head_ << "." << std::endl;
  std::cout << "Tail text: <<" << Tail() << ">>" << std::endl;
}

auto IRC::Parser::Replicate(std::function<char(CharStream &s)> fun, int n, CharStream &s) -> std::string {
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

auto IRC::Parser::Eof(CharStream &s) -> void {
  if (s.Remaining() > 0)
    throw EOFException(s.Location());
  return;
}

auto IRC::Parser::Satisfy(std::function<bool(char)> predicate, CharStream& s) -> char {
  const auto peeked = s.Peek();
  if (predicate(peeked))
    return s.Consume();
  else
    throw MatchFailureException(s.Location(), peeked);
}

auto IRC::Parser::OneOf(std::string options, CharStream& s) -> char {
  return Satisfy([&options](char c) { return options.find(c) != std::string::npos; }, s);
}

auto IRC::Parser::ConsumeWhile(std::function<bool(char)> predicate, CharStream &s)
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

auto IRC::Parser::ConsumeWhile1(std::function<bool(char)> predicate, CharStream &s) -> std::string {
  std::string accum;
  accum += Satisfy(predicate, s);
  accum += ConsumeWhile(predicate, s);
  return accum;
}

auto IRC::Parser::ParseAlpha(CharStream &s) -> char {
  return Satisfy([](char c) { return std::isalpha(c); }, s);
}

auto IRC::Parser::ParseDigit(CharStream &s) -> char {
  return Satisfy([](char c) { return std::isdigit(c); }, s);
}

auto IRC::Parser::ParseSymbol(char target, CharStream &s) -> char {
  return Satisfy([&target](char c) { return target == c; }, s);
}

auto IRC::Parser::ParseWhitespace(CharStream &s) -> void {
  ConsumeWhile([](char c) { return c == ' '; }, s);
}

auto IRC::Parser::ParseWord(CharStream &s) -> std::string {
  return ConsumeWhile1([](char c){ return std::isalpha(c); }, s);
}

auto IRC::Parser::ParseString(std::string expected, CharStream& s) -> std::string {
  for (size_t i = 0; i < expected.size(); i++) {
    ParseSymbol(expected[i], s);
  }

  return expected;
}
