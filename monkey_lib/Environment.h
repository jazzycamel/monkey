#ifndef MONKEY_ENVIRONMENT_H
#define MONKEY_ENVIRONMENT_H

#include "Object.h"
#include <memory>
#include <string>
#include <unordered_map>

class Environment : public std::enable_shared_from_this<Environment> {
public:
  void set(const std::string &name, std::shared_ptr<Object> value);
  std::shared_ptr<Object> get(const std::string &name);
  std::shared_ptr<Environment> createEnclosedEnvironment();
  friend std::ostream &operator<<(std::ostream &os,
                                  Environment const &environment);

private:
  std::unordered_map<std::string, std::shared_ptr<Object>> _store;
  std::shared_ptr<Environment> _outer;
};

#endif // MONKEY_ENVIRONMENT_H
