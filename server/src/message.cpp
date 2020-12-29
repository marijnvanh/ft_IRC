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
    s.debug_stream();
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

  accum += satisfy(predicateExclude(std::vector<char>{'\n','\r','\0',' ',':'}), s);
  accum += consumeWhile(predicateExclude(std::vector<char>{'\n','\r','\0',' '}), s);

  return accum;
}

auto ft_irc::parseTrailing(CharStream& s) -> std::string {
  return consumeWhile(predicateExclude(std::vector<char>{'\n','\r','\0'}), s);
}
