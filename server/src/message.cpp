#include "../include/message.hpp"

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

auto ft_irc::parseRawMessage(CharStream& s) -> RawMessage {
  RawMessage rawMessage;

  rawMessage.prefix = maybe<RawPrefix>([](CharStream& s) {
    parseSymbol(':', s);
    auto prefix = parsePrefix(s);
    parseWhitespace(s);
    return prefix;
  }, s);
  rawMessage.command.name = parseCommandId(s);
  rawMessage.command.parameters = parseParams(s);
  return rawMessage;
}

auto ft_irc::parsePrefix(CharStream &s) -> RawPrefix {
  RawPrefix rawPrefix;

  rawPrefix.name = parseNickname(s);
  rawPrefix.hostname = maybe<Hostname>([](CharStream& s) {
    parseSymbol('@', s);
    return parseHostname(s);
  }, s);
  rawPrefix.username = maybe<std::string>([](CharStream& s) {
    parseSymbol('!', s);
    return parseNickname(s); // TODO: This should probably be *more* lenient
  }, s);
  return rawPrefix;
}

auto ft_irc::parseCommandId(CharStream& s) -> std::string {
  try {
    return attempt<std::string>(parseWord, s);
  } catch (ParseException& e) {
    return replicate(parseDigit, 3, s);
  }
}

auto ft_irc::parseParams(CharStream& s) -> std::vector<std::string> {
  std::vector<std::string> params;

  std::cout << "Parsing params..." << std::endl;

  for(;;) {
    parseWhitespace(s);
    if (s.Peek() == ':') {
      params.push_back(parseTrailing(s));
      return params;
    } else {
      params.push_back(parseMiddle(s));
    }
  }

  throw MatchFailureException(s.Location(), s.Peek());
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
