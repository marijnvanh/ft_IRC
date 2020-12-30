#include "../include/message.hpp"

auto ft_irc::parseCommandId(CharStream &s) -> std::string {
  try {
    return attempt<std::string>(parseWord, s);
  } catch (ParseException &e) {
    return replicate(parseDigit, 3, s);
  }
}

auto ft_irc::parseParams(CharStream& s) -> std::vector<std::string> {
  std::vector<std::string> params;

  for(;;) {
    parseWhitespace(s);
    if (s.peek() == ':') {
      params.push_back(parseTrailing(s));
      return params;
    } else {
      params.push_back(parseMiddle(s));
    }
  }

  throw MatchFailureException(s.location(), s.peek());
}

auto predicateExclude(std::vector<char> blacklist) -> std::function<bool(char)> {
  return [&blacklist](const char c) {
    return std::find(blacklist.begin(), blacklist.end(), c) == blacklist.end();
  };
}

auto ft_irc::parseMiddle(CharStream& s) -> std::string {
  std::string accum;

  accum += satisfy(predicateExclude({'\n','\r','\0',' ',':'}), s);
  accum += consumeWhile(predicateExclude({'\n','\r','\0',' '}), s);

  return accum;
}

auto ft_irc::parseTrailing(CharStream& s) -> std::string {
  return consumeWhile(predicateExclude({'\n','\r','\0'}), s);
}

auto isAlpha(char c) -> bool {
  return (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z');
}

auto isDigit(char c) -> bool {
  return c >= '0' && c <= '9';
}

auto isSpecial(char c) -> bool {
  return (c == '-')
      || (c == '[')
      || (c == ']')
      || (c == '\\')
      || (c == '`')
      || (c == '^')
      || (c == '{')
      || (c == '}');
}

// TODO: implement this in a better (read: more strict, see RFC 952) way
auto ft_irc::parseHostname(CharStream& s) -> Hostname {
  return Hostname(consumeWhile1([](char c){
    return isAlpha(c)
        || c == '-'
        || c == '.'
        || isDigit(c);
  }, s));
}

auto ft_irc::parseNickname(CharStream& s) -> std::string {
  std::string accum;

  accum += satisfy(isAlpha, s);
  accum += consumeWhile([](char c){
    return isAlpha(c)
        || isDigit(c)
        || isSpecial(c);
  }, s);

  return accum;
}
