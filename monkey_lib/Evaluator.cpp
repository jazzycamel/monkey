#include "Evaluator.h"

#include "utilities.h"
#include <string>

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