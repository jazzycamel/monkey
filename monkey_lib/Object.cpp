#include "Object.h"

#include <utility>

IntegerObject::IntegerObject(int64_t value) : value(value) {}
ObjectType IntegerObject::type() { return INTEGER_OBJ; }
std::string IntegerObject::inspect() { return std::to_string(value); }

BooleanObject::BooleanObject(bool value) : value(value) {}
ObjectType BooleanObject::type() { return BOOLEAN_OBJ; }
std::string BooleanObject::inspect() { return value ? "true" : "false"; }

ObjectType NullObject::type() { return NULL_OBJ; }
std::string NullObject::inspect() { return "null"; }

ReturnValueObject::ReturnValueObject(std::shared_ptr<Object> value)
    : value(std::move(value)) {}
ObjectType ReturnValueObject::type() { return RETURN_VALUE_OBJ; }
std::string ReturnValueObject::inspect() { return value->inspect(); }