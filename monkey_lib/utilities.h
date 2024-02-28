#ifndef MONKEY_UTILITIES_H
#define MONKEY_UTILITIES_H

#include <iostream>
#include <memory>
#include <string>

std::string getCurrentUser();

template <typename... Args>
std::string string_format(const std::string &format, Args... args) {
  int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
  if (size_s <= 0)
    throw std::runtime_error("Error during formatting.");
  auto size = static_cast<size_t>(size_s);
  std::unique_ptr<char[]> buffer(new char[size]);
  std::snprintf(buffer.get(), size, format.c_str(), args...);
  return {buffer.get(), buffer.get() + size - 1};
}

#endif // MONKEY_UTILITIES_H
