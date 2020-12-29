
#include "../include/message.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

int main() {
  using namespace ft_irc;

  std::string source = "hello world :foo";
  auto cs = CharStream::from_string(source);

  try {
    auto res = parseParams(cs);
    std::cout << "Parsed: " << res.size() << std::endl;
  } catch (ParseException& e) {
    e.explain(cs);
  }
  return 0;
}
