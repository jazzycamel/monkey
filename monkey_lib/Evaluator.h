#ifndef MONKEY_EVALUATOR_H
#define MONKEY_EVALUATOR_H

#include "AST.h"
#include "Environment.h"
#include "Object.h"
#include <memory>

class Evaluator {
public:
  explicit Evaluator(const std::shared_ptr<Environment> &environment);
  std::shared_ptr<Object> evaluate(const std::shared_ptr<Node> &node);

private:
  std::shared_ptr<Object>
  _evaluateProgram(const std::vector<std::shared_ptr<Statement>> &statements);
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
  _evaluateStringInfixExpression(const std::string &op,
                                 const std::shared_ptr<Object> &left,
                                 const std::shared_ptr<Object> &right);
  std::shared_ptr<Object>
  _evaluateIfExpression(const std::shared_ptr<IfExpression> &ie);
  std::shared_ptr<Object>
  _evaluateBlockStatement(const std::shared_ptr<BlockStatement> &block);
  std::shared_ptr<Object>
  _evaluateIdentifier(const std::shared_ptr<Identifier> &node);
  std::shared_ptr<Object> _evaluateFunctionLiteral(
      const std::shared_ptr<FunctionLiteralExpression> &node);
  std::shared_ptr<Object>
  _evaluateCallExpression(const std::shared_ptr<CallExpression> &node);
  std::vector<std::shared_ptr<Object>>
  _evaluateExpressions(std::vector<std::shared_ptr<Expression>> arguments);
  std::shared_ptr<Object>
  _applyFunction(const std::shared_ptr<Object> &function,
                 const std::vector<std::shared_ptr<Object>> &arguments);
  std::shared_ptr<Environment> _extendFunctionEnvironment(
      const std::shared_ptr<FunctionObject> &function,
      const std::vector<std::shared_ptr<Object>> &arguments);
  std::shared_ptr<Object>
  _unwrapReturnValue(const std::shared_ptr<Object> &object);

  static bool _isTruthy(const std::shared_ptr<Object> &obj);

  template <typename... Args>
  std::shared_ptr<Object> _newError(const std::string &format, Args &&...args);
  static bool _isError(const std::shared_ptr<Object> &obj);

  std::shared_ptr<Environment> _environment;
};

#endif // MONKEY_EVALUATOR_H
