#ifndef MONKEY_EVALUATOR_H
#define MONKEY_EVALUATOR_H

#include "AST.h"
#include "Object.h"
#include <memory>

class Evaluator {
public:
  std::shared_ptr<Object> evaluate(const std::shared_ptr<Node> &node);

private:
  std::shared_ptr<Object> _evaluateStatements(
      const std::vector<std::shared_ptr<Statement>> &statements);
  std::shared_ptr<Object>
  _evaluatePrefixExpression(const std::string &op,
                            const std::shared_ptr<Object> &right);
  std::shared_ptr<Object>
  _evaluateBangOperatorExpression(const std::shared_ptr<Object> &right);
  std::shared_ptr<Object>
  _evaluateMinusPrefixOperatorExpression(const std::shared_ptr<Object> &right);
  std::shared_ptr<Object>
  _evaluateInfixExpression(const std::string &op,
                           const std::shared_ptr<Object> &left,
                           const std::shared_ptr<Object> &right);
  std::shared_ptr<Object>
  _evaluateIntegerInfixExpression(const std::string &op,
                                  const std::shared_ptr<Object> &left,
                                  const std::shared_ptr<Object> &right);
  std::shared_ptr<Object>
  _evaluateIfExpression(const std::shared_ptr<IfExpression> &ie);

  static bool isTruthy(const std::shared_ptr<Object> &obj);
};

const auto NULL_ = std::make_shared<NullObject>();
const auto TRUE_ = std::make_shared<BooleanObject>(true);
const auto FALSE_ = std::make_shared<BooleanObject>(false);

#endif // MONKEY_EVALUATOR_H
