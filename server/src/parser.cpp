#include "parser.hpp"

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

auto ft_irc::parser::replicate(std::function<char(CharStream &s)> fun, int n, CharStream &s)
  -> std::string {
  std::string accum("");

  for (auto i = 0; i < n; i++) {
    try {
      accum += fun(s);
    } catch(ParseException& e) {
      throw e;
    }
  }
  return accum;
}

auto ft_irc::parser::some(std::function<char(CharStream &s)> fun, CharStream &s)
  -> std::string {
  std::string accum("");

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

auto ft_irc::parser::eof(CharStream &s) -> void {
  if (s.Remaining().size() > 0)
    throw EOFException(s.Location());
  return;
}

auto ft_irc::parser::satisfy(std::function<bool(char)> predicate, CharStream& s) -> char {
  const auto peeked = s.Peek();
  if (predicate(peeked))
    return s.Consume();
  else
    throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::oneOf(std::string options, CharStream& s) -> char {
  const auto peeked = s.Peek();
  if (options.find(peeked) == std::string::npos)
    throw MatchFailureException(s.Location(), peeked);
  return s.Consume();
}

auto ft_irc::parser::consumeWhile(std::function<bool(char)> predicate, CharStream &s)
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

auto ft_irc::parser::consumeWhile1(std::function<bool(char)> predicate, CharStream &s)
  -> std::string {
  std::string accum;
  accum += satisfy(predicate, s);
  accum += consumeWhile(predicate, s);
  return accum;
}

auto ft_irc::parser::parseAlpha(CharStream &s) -> char {
    char peeked = s.Peek();
    if ((peeked >= 'a' && peeked <= 'z') ||
        (peeked >= 'A' && peeked <= 'Z'))
        return s.Consume();
    else
        throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::parseDigit(CharStream &s) -> char {
  char peeked = s.Peek();
  if (peeked >= '0' && peeked <= '9')
    return s.Consume();
  else
    throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::parseSymbol(char c, CharStream &s) -> char {
    char peeked = s.Peek();
    if (peeked == c)
        return s.Consume();
    else
        throw MatchFailureException(s.Location(), peeked);
}

auto ft_irc::parser::parseWhitespace(CharStream &s) -> void {
    try {
        for (;;) {
            char peeked = s.Peek();
            if (peeked == ' ')
                s.Consume();
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
    std::string accum;

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
