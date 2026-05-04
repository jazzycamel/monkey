#include "Object.h"

#include <memory>
#include <utility>

ErrorObject::ErrorObject(std::string message) : message(std::move(message)) {}
ObjectType ErrorObject::type() { return ERROR_OBJ; }
std::string ErrorObject::inspect() { return "ERROR: " + message; }

IntegerObject::IntegerObject(int64_t value) : value(value) {}
ObjectType IntegerObject::type() { return INTEGER_OBJ; }
std::string IntegerObject::inspect() { return std::to_string(value); }

StringObject::StringObject(std::string value) : value(std::move(value)) {}
ObjectType StringObject::type() { return STRING_OBJ; }
std::string StringObject::inspect() { return value; }

BooleanObject::BooleanObject(bool value) : value(value) {}
ObjectType BooleanObject::type() { return BOOLEAN_OBJ; }
std::string BooleanObject::inspect() { return value ? "true" : "false"; }

ObjectType NullObject::type() { return NULL_OBJ; }
std::string NullObject::inspect() { return "null"; }

ReturnValueObject::ReturnValueObject(std::shared_ptr<Object> value)
    : value(std::move(value)) {}
ObjectType ReturnValueObject::type() { return RETURN_VALUE_OBJ; }
std::string ReturnValueObject::inspect() { return value->inspect(); }

FunctionObject::FunctionObject(
    IdentifierPtrVec parameters,
    BlockStatementPtr body,
    std::shared_ptr<Environment> environment)
    : parameters(std::move(parameters)), body(std::move(body)),
      environment(std::move(environment)) {}

ObjectType FunctionObject::type() { return FUNCTION_OBJ; }
std::string FunctionObject::inspect() {
  std::string out = "fn(";
  for (size_t i = 0; i < parameters.size(); i++) {
    out += parameters[i]->string();
    if (i != parameters.size() - 1) {
      out += ", ";
    }
  }
  out += ") {\n";
  out += body->string();
  out += "\n}";
  return out;
}

BuiltinObject::BuiltinObject(BuiltinFunction value) : value(std::move(value)) {}
ObjectType BuiltinObject::type() { return BUILTIN_OBJ; }
std::string BuiltinObject::inspect() { return "builtin function"; }

ArrayObject::ArrayObject(std::vector<std::shared_ptr<Object>> elements) : elements(std::move(elements)) {}
ObjectType ArrayObject::type() { return ARRAY_OBJ; }
std::string ArrayObject::inspect() {
  std::string out = "[";
  for (size_t i = 0; i < elements.size(); i++) {
    out += elements[i]->inspect();
    if (i != elements.size() -1 ) out += ", ";
  }
  out += "]";
  return out;
}