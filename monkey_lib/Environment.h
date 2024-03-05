#ifndef MONKEY_ENVIRONMENT_H
#define MONKEY_ENVIRONMENT_H

#include "Object.h"
#include <string>
#include <unordered_map>

class Environment {
public:
  void set(const std::string &name, std::shared_ptr<Object> value);
  std::shared_ptr<Object> get(const std::string &name);
  //  std::shared_ptr<Environment> extend();
  //  std::shared_ptr<Environment> extend(std::vector<std::string> names,
  //                                  std::vector<std::shared_ptr<Object>>
  //                                  values);

private:
  std::unordered_map<std::string, std::shared_ptr<Object>> _store;
  // std::shared_ptr<Environment> _outer;
};

#endif // MONKEY_ENVIRONMENT_H
