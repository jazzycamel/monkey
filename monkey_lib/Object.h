#ifndef MONKEY_OBJECT_H
#define MONKEY_OBJECT_H

#include <memory>
#include <string>

typedef std::string ObjectType;

const ObjectType INTEGER_OBJ = "INTEGER", BOOLEAN_OBJ = "BOOLEAN",
                 NULL_OBJ = "NULL", RETURN_VALUE_OBJ = "RETURN_VALUE";

class Object {
public:
  virtual ObjectType type() = 0;
  virtual std::string inspect() = 0;
};

class IntegerObject : public Object {
public:
  explicit IntegerObject(int64_t value = 0);
  ObjectType type() override;
  std::string inspect() override;

  int64_t value;
};

class BooleanObject : public Object {
public:
  explicit BooleanObject(bool value = false);
  ObjectType type() override;
  std::string inspect() override;

  bool value;
};

class NullObject : public Object {
public:
  ObjectType type() override;
  std::string inspect() override;
};

class ReturnValueObject : public Object {
public:
  explicit ReturnValueObject(std::shared_ptr<Object> value);
  ObjectType type() override;
  std::string inspect() override;

  std::shared_ptr<Object> value;
};

#endif // MONKEY_OBJECT_H
