#include "Environment.h"

#include <utility>

void Environment::set(const std::string &name, std::shared_ptr<Object> value) {
  _store[name] = std::move(value);
}

std::shared_ptr<Object> Environment::get(const std::string &name) {
  if (_store.find(name) != _store.end()) {
    return _store[name];
  }
  //  if (_outer) {
  //    return _outer->get(std::move(name));
  //  }
  //  return NULL_;
  return nullptr;
}