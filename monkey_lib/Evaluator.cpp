#include "Evaluator.h"

#include "utilities.h"
#include <string>

Evaluator::Evaluator(const std::shared_ptr<Environment> &environment)
    : _environment(environment) {}

std::shared_ptr<Object> Evaluator::evaluate(const std::shared_ptr<Node> &node) {
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
  default:
    return nullptr;
  }
}

std::shared_ptr<Object> Evaluator::_evaluateProgram(
    const std::vector<std::shared_ptr<Statement>> &statements) {
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
Evaluator::_evaluateIfExpression(const std::shared_ptr<IfExpression> &ie) {
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
    const std::shared_ptr<BlockStatement> &block) {
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
Evaluator::_evaluateIdentifier(const std::shared_ptr<Identifier> &node) {
  auto value = _environment->get(node->value);
  if (value->type() != NULL_OBJ) {
    return value;
  }
  return _newError("identifier not found: %s", node->value.c_str());
}

std::shared_ptr<Object> Evaluator::_evaluateFunctionLiteral(
    const std::shared_ptr<FunctionLiteralExpression> &node) {
  return std::make_shared<FunctionObject>(node->parameters, node->body,
                                          _environment);
}

std::vector<std::shared_ptr<Object>> Evaluator::_evaluateExpressions(
    std::vector<std::shared_ptr<Expression>> arguments) {
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
    const std::shared_ptr<CallExpression> &node) {
  auto function = evaluate(node->function);
  if (_isError(function))
    return function;
  auto arguments = _evaluateExpressions(node->arguments);
  if (arguments.size() == 1 && _isError(arguments[0]))
    return arguments[0];
  return _applyFunction(function, arguments);
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