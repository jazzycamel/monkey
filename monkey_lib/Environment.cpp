#include "Environment.h"

#include <iostream>
#include <utility>

void Environment::set(const std::string &name, std::shared_ptr<Object> value) {
  _store[name] = std::move(value);
}

std::shared_ptr<Object> Environment::get(const std::string &name) {
  if (_store.find(name) != _store.end()) {
    return _store[name];
  }
  if (_outer) {
    return _outer->get(name);
  }
  return NULL_;
}

std::shared_ptr<Environment> Environment::createEnclosedEnvironment() {
  auto environment = std::make_shared<Environment>();
  environment->_outer = shared_from_this();
  return environment;
}

std::ostream &operator<<(std::ostream &os, Environment const &environment) {
  for (auto const &x : environment._store) {
    os << x.first << "=" << x.second->inspect() << std::endl;
  }
  if (environment._outer) {
    for (auto const &x : environment._outer->_store) {
      os << "outer: " << x.first << "=" << x.second->inspect() << std::endl;
    }
  }
  return os;
}