#include "Object.h"

IntegerObject::IntegerObject(int64_t value) : value(value) {}
ObjectType IntegerObject::type() { return INTEGER_OBJ; }
std::string IntegerObject::inspect() { return std::to_string(value); }

BooleanObject::BooleanObject(bool value) : value(value) {}
ObjectType BooleanObject::type() { return BOOLEAN_OBJ; }
std::string BooleanObject::inspect() { return value ? "true" : "false"; }

ObjectType NullObject::type() { return NULL_OBJ; }
std::string NullObject::inspect() { return "null"; }