#include "parser.hpp"

auto ParseException::explain(const CharStream& stream) const -> void {
  std::cout << "Parse Exception: "
            << what()
            << ", at "
            << location
            << ". Remaning: '"
            << stream.remaining()
            << "'."
            << std::endl;
}

auto CharStream::consume_many(const size_t count) -> std::string {
  if (read_head_ + count - 1 >= (*stream_).size())
    throw StreamEndException(read_head_);
  read_head_ += count;
  return (*stream_).substr(read_head_, read_head_ + count);
}

auto CharStream::consume() -> char {
  if (read_head_ >= (*stream_).size())
    throw StreamEndException(read_head_);
  read_head_++;
  return (*stream_)[read_head_ - 1];
}

auto CharStream::peek() const -> char {
  if (read_head_ >= (*stream_).size())
    throw StreamEndException(read_head_);
  return (*stream_)[read_head_];
}

auto CharStream::remaining() const -> std::string {
  return (*stream_)
    .substr(read_head_, (*stream_).size() - read_head_);
}

auto CharStream::location() const -> size_t {
  return read_head_;
}
