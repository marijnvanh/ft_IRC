#include "RawMessage.hpp"
#include <cctype>

auto IsSpecial(char c) -> bool {
  return (c == '-')
      || (c == '[')
      || (c == ']')
      || (c == '\\')
      || (c == '`')
      || (c == '^')
      || (c == '{')
      || (c == '}');
}

auto ft_irc::ParseRawMessage(CharStream& s) -> RawMessage {
  RawMessage rawMessage;

  rawMessage.prefix = Maybe<RawPrefix>([](CharStream& s) {
    ParseSymbol(':', s);
    auto prefix = ParsePrefix(s);
    ParseWhitespace(s);
    return prefix;
  }, s);
  rawMessage.command.name = ParseCommandId(s);
  rawMessage.command.parameters = ParseParams(s);
  return rawMessage;
}

auto ft_irc::ParsePrefix(CharStream &s) -> RawPrefix {
  RawPrefix rawPrefix;

  rawPrefix.name = ParseNickname(s);
  rawPrefix.hostname = Maybe<Hostname>([](CharStream& s) {
    ParseSymbol('@', s);
    return ParseHostname(s);
  }, s);
  rawPrefix.username = Maybe<std::string>([](CharStream& s) {
    ParseSymbol('!', s);
    return ParseNickname(s); // TODO: This should probably be *more* lenient
  }, s);
  return rawPrefix;
}

auto ft_irc::ParseCommandId(CharStream& s) -> std::string {
  try {
    return Attempt<std::string>(ParseWord, s);
  } catch (ParseException& e) {
    return Replicate(ParseDigit, 3, s);
  }
}

auto ft_irc::ParseParams(CharStream& s) -> std::vector<std::string> {
  std::vector<std::string> params;

  for(;;) {
    ParseWhitespace(s);
    if (s.Peek() == ':') {
      params.push_back(ParseTrailing(s));
      return params;
    } else {
      params.push_back(ParseMiddle(s));
    }
  }
}

auto PredicateExclude(std::vector<char> blacklist) -> std::function<bool(char)> {
  return [&blacklist](const char c) {
    return std::find(blacklist.begin(), blacklist.end(), c) == blacklist.end();
  };
}

auto ft_irc::ParseMiddle(CharStream& s) -> std::string {
  std::string accum;

  accum += Satisfy(PredicateExclude({'\n','\r','\0',' ',':'}), s);
  accum += ConsumeWhile(PredicateExclude({'\n','\r','\0',' '}), s);

  return accum;
}

auto ft_irc::ParseTrailing(CharStream& s) -> std::string {
  return ConsumeWhile(PredicateExclude({'\n','\r','\0'}), s);
}

// TODO: implement this in a better (read: more strict, see RFC 952) way
auto ft_irc::ParseHostname(CharStream& s) -> Hostname {
  return Hostname(ConsumeWhile1([](char c){
    return std::isalpha(c)
        || c == '-'
        || c == '.'
        || std::isdigit(c);
  }, s));
}

auto ft_irc::ParseNickname(CharStream& s) -> std::string {
  std::string accum;

  accum += Satisfy([](char c) { return std::isalpha(c); }, s);
  accum += ConsumeWhile([](char c){
    return std::isalpha(c)
        || std::isdigit(c)
        || IsSpecial(c);
  }, s);

  return accum;
}
