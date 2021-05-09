#include "RawMessage.h"
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

auto IRC::ParseRawMessage(CharStream& s) -> RawMessage {
  RawMessage rawMessage;

  rawMessage.prefix = Maybe<RawPrefix>([](CharStream& s) {
    ParseSymbol(':', s);
    auto prefix = ParsePrefix(s);
    ParseWhitespace(s);
    return prefix;
  }, s);
  rawMessage.command.name = ParseCommandId(s);
  ParseWhitespace(s);
  if (s.Remaining() > 0) {
      rawMessage.command.parameters = ParseParams(s);
  } else {
      rawMessage.command.parameters = std::vector<std::string>{};
  }
  return rawMessage;
}

/**
 * @brief Parses message prefix defined as: servername | nickname [ !user@host ]
*/
auto IRC::ParsePrefix(CharStream &s) -> RawPrefix {
  RawPrefix rawPrefix;

  auto name = TryParseServername(s);
  if (name)
  {
	  rawPrefix.name = (*name).value;
	  rawPrefix.is_server_origin = true;
	  return rawPrefix;
  }

  rawPrefix.name = ParseNickname(s);
  rawPrefix.username = Maybe<std::string>([](CharStream& s) {
    ParseSymbol('!', s);
    return ParseNickname(s); // TODO: This should probably be *more* lenient
  }, s);
  rawPrefix.hostname = Maybe<Hostname>([](CharStream& s) {
    ParseSymbol('@', s);
    return ParseHostname(s);
  }, s);
  return rawPrefix;
}

auto IRC::ParseCommandId(CharStream& s) -> std::string {
  try {
    return Attempt<std::string>(ParseWord, s);
  } catch (ParseException& e) {
    return Replicate(ParseDigit, 3, s);
  }
}

auto IRC::ParseParams(CharStream& s) -> std::vector<std::string> {
  std::vector<std::string> params;

  for(;;) {
    ParseWhitespace(s);
    if (s.Peek() == ':') {
      s.Consume();
      params.push_back(ParseTrailing(s));
      return params;
    } else {
      params.push_back(ParseMiddle(s));
      ParseWhitespace(s);
      if (s.Remaining() == 0) {
        return params;
      }
    }
  }
}

auto PredicateExclude(std::vector<char> blacklist) -> std::function<bool(char)> {
  return [&blacklist](const char c) {
    return std::find(blacklist.begin(), blacklist.end(), c) == blacklist.end();
  };
}

auto IRC::ParseMiddle(CharStream& s) -> std::string {
  std::string accum;

  accum += Satisfy(PredicateExclude({'\n','\r','\0',' ',':'}), s);
  accum += ConsumeWhile(PredicateExclude({'\n','\r','\0',' '}), s);

  return accum;
}

auto IRC::ParseTrailing(CharStream& s) -> std::string {
  return ConsumeWhile(PredicateExclude({'\n','\r','\0'}), s);
}

auto IRC::TryParseServername(CharStream& s) -> std::optional<Hostname> {
	size_t current_head = s.Location();
	auto hostname = ParseHostname(s);

	if (hostname.value.find('.') == std::string::npos)
	{
		s.SetLocation(current_head);
		return (std::nullopt);
	}
	return (hostname);
}

// TODO: implement this in a better (read: more strict, see RFC 952) way
auto IRC::ParseHostname(CharStream& s) -> Hostname {
  return Hostname(ConsumeWhile1([](char c){
    return std::isalpha(c)
        || c == '-'
        || c == '.'
        || std::isdigit(c);
  }, s));
}

auto IRC::ParseNickname(CharStream& s) -> std::string {
  std::string accum;

  accum += Satisfy([](char c) { return std::isalpha(c); }, s);
  accum += ConsumeWhile([](char c){
    return std::isalpha(c)
        || std::isdigit(c)
        || IsSpecial(c);
  }, s);

  return accum;
}
