#include "../include/message.hpp"

auto ft_irc::parseCommandId(CharStream &s) -> std::string {
  try {
    return attempt<std::string>(parseWord, s);
  } catch (ParseException &e) {
    return replicate(parseDigit, 3, s);
  }
}

auto ft_irc::parseParams(CharStream& s) -> std::vector<std::string> {
  (void)s;
  return std::vector<std::string>{};
}
