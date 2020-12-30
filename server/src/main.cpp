
#include "../include/message.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

int main() {
  using namespace ft_irc;

  std::string source = "foo123[pog]";
  auto cs = CharStream::from_string(source);

  try {
    auto res = parseNickname(cs);
    eof(cs);
    std::cout << "Parsed: " << res << std::endl;
    std::cout << "Remain: " << cs.remaining() << std::endl;
  } catch (ParseException& e) {
    e.explain(cs);
  }
  return 0;
}
