#include "Evaluator.h"

#include <iostream>

std::shared_ptr<Object> Evaluator::evaluate(const std::shared_ptr<Node> &node) {
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
    return _evaluatePrefixExpression(
        std::dynamic_pointer_cast<PrefixExpression>(node)->operator_,
        evaluate(std::dynamic_pointer_cast<PrefixExpression>(node)->right));
  case NodeType::INFIX_EXPRESSION:
    return _evaluateInfixExpression(
        std::dynamic_pointer_cast<InfixExpression>(node)->operator_,
        evaluate(std::dynamic_pointer_cast<InfixExpression>(node)->left),
        evaluate(std::dynamic_pointer_cast<InfixExpression>(node)->right));
  case NodeType::BLOCK_STATEMENT:
    return _evaluateBlockStatement(
        std::dynamic_pointer_cast<BlockStatement>(node));
  case NodeType::IF_EXPRESSION:
    return _evaluateIfExpression(std::dynamic_pointer_cast<IfExpression>(node));
  case NodeType::RETURN_STATEMENT:
    return std::make_shared<ReturnValueObject>(evaluate(
        std::dynamic_pointer_cast<ReturnStatement>(node)->returnValue));
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
  return NULL_;
}

std::shared_ptr<Object> Evaluator::_evaluateBangOperatorExpression(
    const std::shared_ptr<Object> &right) {
  return (right == FALSE_ || right == NULL_) ? TRUE_ : FALSE_;
}

std::shared_ptr<Object> Evaluator::_evaluateMinusPrefixOperatorExpression(
    const std::shared_ptr<Object> &right) {
  if (right->type() != INTEGER_OBJ) {
    return NULL_;
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
  }
  return NULL_;
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
  return NULL_;
}

std::shared_ptr<Object>
Evaluator::_evaluateIfExpression(const std::shared_ptr<IfExpression> &ie) {
  auto condition = evaluate(ie->condition);
  if (isTruthy(condition)) {
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
    if (result != NULL_ && result->type() == RETURN_VALUE_OBJ) {
      return result;
    }
  }
  return result;
}

bool Evaluator::isTruthy(const std::shared_ptr<Object> &obj) {
  return !(obj == NULL_ || obj == FALSE_);
}