
#include "../include/message.hpp"
#include "../include/parser.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

int main() {
  using namespace ft_irc;

  std::string source = "321";

  std::string res = parser::run<std::string>(parseCommandId, source);

  std::cout << "Parsed: " << res << std::endl;

  return 0;
}
