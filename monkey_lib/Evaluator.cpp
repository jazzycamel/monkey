#include "Evaluator.h"

#include "AST.h"
#include "Object.h"
#include "utilities.h"
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>



// XXX - Maybe this should be part of the evaluator class?
// We definietely should not be duplicating the error raising mechanism
const std::unordered_map<std::string, BuiltinObject> builtins = {
  {"len", 
    BuiltinObject{[](const std::vector<std::shared_ptr<Object>>& args)->std::shared_ptr<Object>{
      if(args.size() != 1){
        return std::make_shared<ErrorObject>(string_format("wrong number of arguments, got=%d, want=1", args.size()));
      }

      if(args[0]->type() == ARRAY_OBJ){
        auto arrayObject = std::dynamic_pointer_cast<ArrayObject>(args[0]);
        return std::make_shared<IntegerObject>(arrayObject->elements.size());
      }
      if(args[0]->type() == STRING_OBJ){
        auto stringObject = std::dynamic_pointer_cast<StringObject>(args[0]);
        return std::make_shared<IntegerObject>(stringObject->value.length()); 
      }

      return std::make_shared<ErrorObject>(string_format("argument to `len` not supported, got %s", args[0]->type().c_str()));
    }}
  },
  {"first",
    BuiltinObject{[](const std::vector<std::shared_ptr<Object>>& args)->std::shared_ptr<Object>{
      if(args.size() != 1){
        return std::make_shared<ErrorObject>(string_format("wrong number of arguments, got=%d, want=1", args.size()));
      }

      if (args[0]->type() != ARRAY_OBJ){
        return std::make_shared<ErrorObject>(string_format("argument to `first` must be ARRAY, got %s", args[0]->type().c_str()));
      }

      auto arr = std::dynamic_pointer_cast<ArrayObject>(args[0]);
      if (arr->elements.size() > 0) {
        return arr->elements[0];
      }

      return NULL_;
    }}
  },
  {"last",
    BuiltinObject{[](const std::vector<std::shared_ptr<Object>>& args)->std::shared_ptr<Object>{
      if(args.size() != 1){
        return std::make_shared<ErrorObject>(string_format("wrong number of arguments, got=%d, want=1", args.size()));
      }

      if (args[0]->type() != ARRAY_OBJ){
        return std::make_shared<ErrorObject>(string_format("argument to `last` must be ARRAY, got %s", args[0]->type().c_str()));
      }

      auto arr = std::dynamic_pointer_cast<ArrayObject>(args[0]);
      auto length = arr->elements.size();
      if (length > 0) {
        return arr->elements[length-1];
      }

      return NULL_;
    }}
  },
  {"rest",
    BuiltinObject{[](const std::vector<std::shared_ptr<Object>>& args)->std::shared_ptr<Object>{
      if(args.size() != 1){
        return std::make_shared<ErrorObject>(string_format("wrong number of arguments, got=%d, want=1", args.size()));
      }

      if (args[0]->type() != ARRAY_OBJ){
        return std::make_shared<ErrorObject>(string_format("argument to `rest` must be ARRAY, got %s", args[0]->type().c_str()));
      }

      auto arr = std::dynamic_pointer_cast<ArrayObject>(args[0]);
      auto length = arr->elements.size();
      if (length > 0) {
        std::vector<std::shared_ptr<Object>> slice(arr->elements.begin() + 1, arr->elements.end());
        return std::make_shared<ArrayObject>(slice);
      }

      return NULL_;
    }}
  },
  {"push",
    BuiltinObject{[](const std::vector<std::shared_ptr<Object>>& args)->std::shared_ptr<Object>{
      if(args.size() != 2){
        return std::make_shared<ErrorObject>(string_format("wrong number of arguments, got=%d, want=2", args.size()));
      }

      if (args[0]->type() != ARRAY_OBJ){
        return std::make_shared<ErrorObject>(string_format("argument to `push` must be ARRAY, got %s", args[0]->type().c_str()));
      }

      auto arr = std::dynamic_pointer_cast<ArrayObject>(args[0]);      
      arr->elements.push_back(args[1]);
      return std::make_shared<ArrayObject>(arr->elements);
    }}
  }
};


Evaluator::Evaluator(const std::shared_ptr<Environment> &environment)
    : _environment(environment) {}

std::shared_ptr<Object> Evaluator::evaluate(const NodePtr &node) {
  std::shared_ptr<Object> result, result2;

  switch (node->nodeType()) {
  case NodeType::PROGRAM:
    return _evaluateProgram(
        std::dynamic_pointer_cast<Program>(node)->statements);
  case NodeType::EXPRESSION_STATEMENT:
    return evaluate(
        std::dynamic_pointer_cast<ExpressionStatement>(node)->expression);
  case NodeType::INTEGER_LITERAL:
    return std::make_shared<IntegerObject>(
        std::dynamic_pointer_cast<IntegerLiteralExpression>(node)->value);
  case NodeType::STRING_LITERAL:
    return std::make_shared<StringObject>(
        std::dynamic_pointer_cast<StringLiteralExpression>(node)->value);
  case NodeType::BOOLEAN_LITERAL:
    return std::dynamic_pointer_cast<BooleanLiteralExpression>(node)->value
               ? TRUE_
               : FALSE_;
  case NodeType::PREFIX_EXPRESSION:
    result = evaluate(std::dynamic_pointer_cast<PrefixExpression>(node)->right);
    if (_isError(result))
      return result;
    return _evaluatePrefixExpression(
        std::dynamic_pointer_cast<PrefixExpression>(node)->operator_, result);
  case NodeType::INFIX_EXPRESSION:
    result = evaluate(std::dynamic_pointer_cast<InfixExpression>(node)->left);
    if (_isError(result))
      return result;
    result2 = evaluate(std::dynamic_pointer_cast<InfixExpression>(node)->right);
    if (_isError(result2))
      return result2;
    return _evaluateInfixExpression(
        std::dynamic_pointer_cast<InfixExpression>(node)->operator_, result,
        result2);
  case NodeType::BLOCK_STATEMENT:
    return _evaluateBlockStatement(
        std::dynamic_pointer_cast<BlockStatement>(node));
  case NodeType::IF_EXPRESSION:
    return _evaluateIfExpression(std::dynamic_pointer_cast<IfExpression>(node));
  case NodeType::RETURN_STATEMENT:
    result =
        evaluate(std::dynamic_pointer_cast<ReturnStatement>(node)->returnValue);
    if (_isError(result))
      return result;
    return std::make_shared<ReturnValueObject>(result);
  case NodeType::LET_STATEMENT:
    result = evaluate(std::dynamic_pointer_cast<LetStatement>(node)->value);
    if (_isError(result))
      return result;
    _environment->set(
        std::dynamic_pointer_cast<LetStatement>(node)->name->value, result);
    return NULL_;
  case NodeType::IDENTIFIER:
    return _evaluateIdentifier(std::dynamic_pointer_cast<Identifier>(node));
  case NodeType::FUNCTION_LITERAL:
    return _evaluateFunctionLiteral(
        std::dynamic_pointer_cast<FunctionLiteralExpression>(node));
  case NodeType::CALL_EXPRESSION:
    return _evaluateCallExpression(
        std::dynamic_pointer_cast<CallExpression>(node));
  case NodeType::ARRAY_LITERAL: {
    auto array = std::dynamic_pointer_cast<ArrayLiteralExpression>(node);
    auto elements = _evaluateExpressions(array->elements);
    if (elements.size() == 1 && _isError(elements[0])) {
      return elements[0];
    }
    return std::make_shared<ArrayObject>(elements);
  }
  case NodeType::INDEX_EXPRESSION: {
    auto indexExpression = std::dynamic_pointer_cast<IndexExpression>(node);
    auto left = evaluate(indexExpression->left);
    if (_isError(left)) {
      return left;
    }
    auto index = evaluate(indexExpression->index);
    if (_isError(index)) {
      return index;
    }
    return _evaluateIndexExpression(left, index);
  }
  default:
    return nullptr;
  }
}

std::shared_ptr<Object> Evaluator::_evaluateProgram(
    const StatementPtrVec &statements) {
  std::shared_ptr<Object> result;
  for (const auto &statement : statements) {
    result = evaluate(statement);
    if (result->type() == RETURN_VALUE_OBJ) {
      return std::dynamic_pointer_cast<ReturnValueObject>(result)->value;
    } else if (result->type() == ERROR_OBJ) {
      return result;
    }
  }
  return result;
}

std::shared_ptr<Object>
Evaluator::_evaluatePrefixExpression(const std::string &op,
                                     const std::shared_ptr<Object> &right) {
  if (op == "!") {
    return _evaluateBangOperatorExpression(right);
  } else if (op == "-") {
    return _evaluateMinusPrefixOperatorExpression(right);
  }
  return _newError("unknown operator: %s%s", op.c_str(),
                   right->inspect().c_str());
}

std::shared_ptr<Object> Evaluator::_evaluateBangOperatorExpression(
    const std::shared_ptr<Object> &right) {
  return (right == FALSE_ || right == NULL_) ? TRUE_ : FALSE_;
}

std::shared_ptr<Object> Evaluator::_evaluateMinusPrefixOperatorExpression(
    const std::shared_ptr<Object> &right) {
  if (right->type() != INTEGER_OBJ) {
    return _newError("unknown operator: -%s", right->type().c_str());
  }
  auto value = std::dynamic_pointer_cast<IntegerObject>(right)->value;
  return std::make_shared<IntegerObject>(-value);
}

std::shared_ptr<Object>
Evaluator::_evaluateInfixExpression(const std::string &op,
                                    const std::shared_ptr<Object> &left,
                                    const std::shared_ptr<Object> &right) {
  if (left->type() == INTEGER_OBJ && right->type() == INTEGER_OBJ) {
    return _evaluateIntegerInfixExpression(op, left, right);
  } else if (op == "==") {
    return left == right ? TRUE_ : FALSE_;
  } else if (op == "!=") {
    return left != right ? TRUE_ : FALSE_;
  } else if (left->type() == STRING_OBJ && right->type() == STRING_OBJ) {
    return _evaluateStringInfixExpression(op, left, right);

  } else if (left->type() != right->type()) {
    return _newError("type mismatch: %s %s %s", left->type().c_str(),
                     op.c_str(), right->type().c_str());
  }
  return _newError("unknown operator: %s %s %s", left->type().c_str(),
                   op.c_str(), right->type().c_str());
}

std::shared_ptr<Object> Evaluator::_evaluateIntegerInfixExpression(
    const std::string &op, const std::shared_ptr<Object> &left,
    const std::shared_ptr<Object> &right) {
  auto leftValue = std::dynamic_pointer_cast<IntegerObject>(left)->value;
  auto rightValue = std::dynamic_pointer_cast<IntegerObject>(right)->value;

  if (op == "+") {
    return std::make_shared<IntegerObject>(leftValue + rightValue);
  } else if (op == "-") {
    return std::make_shared<IntegerObject>(leftValue - rightValue);
  } else if (op == "*") {
    return std::make_shared<IntegerObject>(leftValue * rightValue);
  } else if (op == "/") {
    return std::make_shared<IntegerObject>(leftValue / rightValue);
  } else if (op == "<") {
    return leftValue < rightValue ? TRUE_ : FALSE_;
  } else if (op == ">") {
    return leftValue > rightValue ? TRUE_ : FALSE_;
  } else if (op == "==") {
    return leftValue == rightValue ? TRUE_ : FALSE_;
  } else if (op == "!=") {
    return leftValue != rightValue ? TRUE_ : FALSE_;
  }
  return _newError("unknown operator: %s %s %s", left->inspect().c_str(),
                   op.c_str(), right->inspect().c_str());
}

std::shared_ptr<Object> Evaluator::_evaluateStringInfixExpression(
    const std::string &op, const std::shared_ptr<Object> &left,
    const std::shared_ptr<Object> &right) {
  if (op != "+") {
    return _newError("unknown operator: %s %s %s", left->type().c_str(),
                     op.c_str(), right->type().c_str());
  }
  auto leftValue = std::dynamic_pointer_cast<StringObject>(left)->value;
  auto rightValue = std::dynamic_pointer_cast<StringObject>(right)->value;
  return std::make_shared<StringObject>(leftValue + rightValue);
}

std::shared_ptr<Object>
Evaluator::_evaluateIfExpression(const IfExpressionPtr &ie) {
  auto condition = evaluate(ie->condition);
  if (_isError(condition))
    return condition;
  if (_isTruthy(condition)) {
    return evaluate(ie->consequence);
  } else if (ie->alternative != nullptr) {
    return evaluate(ie->alternative);
  }
  return NULL_;
}

std::shared_ptr<Object> Evaluator::_evaluateBlockStatement(
    const BlockStatementPtr &block) {
  std::shared_ptr<Object> result;
  for (const auto &statement : block->statements) {
    result = evaluate(statement);
    if (result->type() == RETURN_VALUE_OBJ || result->type() == ERROR_OBJ) {
      return result;
    }
  }
  return result;
}

std::shared_ptr<Object>
Evaluator::_evaluateIdentifier(const IdentifierPtr &node) {
  auto value = _environment->get(node->value);
  if (value->type() != NULL_OBJ) {
    return value;
  }

  if (builtins.contains(node->value)){
    return std::make_shared<BuiltinObject>(builtins.at(node->value));
  }

  return _newError("identifier not found: %s", node->value.c_str());
}

std::shared_ptr<Object> Evaluator::_evaluateFunctionLiteral(
    const FunctionLiteralExpressionPtr &node) {
  return std::make_shared<FunctionObject>(node->parameters, node->body,
                                          _environment);
}

std::vector<std::shared_ptr<Object>> Evaluator::_evaluateExpressions(
    ExpressionPtrVec arguments) {
  std::vector<std::shared_ptr<Object>> result;

  for (auto &argument : arguments) {
    auto evaluated = evaluate(argument);
    if (_isError(evaluated))
      return {evaluated};
    result.push_back(evaluated);
  }

  return result;
}

std::shared_ptr<Object> Evaluator::_evaluateCallExpression(
    const CallExpressionPtr &node) {
  auto function = evaluate(node->function);
  if (_isError(function))
    return function;
  auto arguments = _evaluateExpressions(node->arguments);
  if (arguments.size() == 1 && _isError(arguments[0]))
    return arguments[0];
  return _applyFunction(function, arguments);
}

std::shared_ptr<Object> Evaluator::_evaluateIndexExpression(std::shared_ptr<Object> left, std::shared_ptr<Object> index){
  if (left->type() == ARRAY_OBJ && index->type() == INTEGER_OBJ) {
    return _evaluateArrayIndexExpression(left, index);
  }
  return _newError("index operator not supported: %s", left->type().c_str());
}

std::shared_ptr<Object> Evaluator::_evaluateArrayIndexExpression(std::shared_ptr<Object> array, std::shared_ptr<Object> index) {
  auto arrayObject = std::dynamic_pointer_cast<ArrayObject>(array);
  auto indexObject = std::dynamic_pointer_cast<IntegerObject>(index);
  auto idx = indexObject->value;
  auto max = arrayObject->elements.size() - 1;

  if (idx < 0 || idx > max) return NULL_;
  return arrayObject->elements[idx];
}

std::shared_ptr<Object> Evaluator::_applyFunction(
    const std::shared_ptr<Object> &function,
    const std::vector<std::shared_ptr<Object>> &arguments) {
  if (function->type() == FUNCTION_OBJ) {
    auto fn = std::dynamic_pointer_cast<FunctionObject>(function);
    auto extendedEnv = _extendFunctionEnvironment(fn, arguments);
    Evaluator _evaluator(extendedEnv);
    auto evaluated = _evaluator.evaluate(fn->body);
    return _unwrapReturnValue(evaluated);
  }
  if (function->type() == BUILTIN_OBJ) {
    auto builtin = std::dynamic_pointer_cast<BuiltinObject>(function);
    return builtin->value(arguments);
  }
  return _newError("not a function: %s", function->type().c_str());
}

std::shared_ptr<Environment> Evaluator::_extendFunctionEnvironment(
    const std::shared_ptr<FunctionObject> &function,
    const std::vector<std::shared_ptr<Object>> &arguments) {

  auto environment = function->environment->createEnclosedEnvironment();
  for (size_t i = 0; i < function->parameters.size(); i++) {
    function->environment->set(function->parameters[i]->value, arguments[i]);
  }
  return environment;
}

std::shared_ptr<Object>
Evaluator::_unwrapReturnValue(const std::shared_ptr<Object> &object) {
  if (object->type() == RETURN_VALUE_OBJ) {
    return std::dynamic_pointer_cast<ReturnValueObject>(object)->value;
  }
  return object;
}

bool Evaluator::_isTruthy(const std::shared_ptr<Object> &obj) {
  return !(obj == NULL_ || obj == FALSE_);
}

template <typename... Args>
std::shared_ptr<Object> Evaluator::_newError(const std::string &format_,
                                             Args &&...args) {

  return std::make_shared<ErrorObject>(string_format(format_, args...));
}

bool Evaluator::_isError(const std::shared_ptr<Object> &obj) {
  return obj->type() == ERROR_OBJ;
}