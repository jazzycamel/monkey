#ifndef MONKEY_EVALUATOR_H
#define MONKEY_EVALUATOR_H

#include "AST.h"
#include "Environment.h"
#include "Object.h"
#include <memory>
#include <unordered_map>


extern const std::unordered_map<std::string, BuiltinObject> builtins;


class Evaluator {
public:
  explicit Evaluator(const std::shared_ptr<Environment> &environment);
  std::shared_ptr<Object> evaluate(const NodePtr &node);

private:
  std::shared_ptr<Object>
  _evaluateProgram(const StatementPtrVec &statements);
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
  _evaluateIfExpression(const IfExpressionPtr &ie);
  std::shared_ptr<Object>
  _evaluateBlockStatement(const BlockStatementPtr &block);
  std::shared_ptr<Object>
  _evaluateIdentifier(const IdentifierPtr &node);
  std::shared_ptr<Object> _evaluateFunctionLiteral(
      const FunctionLiteralExpressionPtr &node);
  std::shared_ptr<Object>
  _evaluateCallExpression(const CallExpressionPtr &node);
  std::shared_ptr<Object> 
      _evaluateIndexExpression(std::shared_ptr<Object> left, std::shared_ptr<Object> index);
  std::vector<std::shared_ptr<Object>>
  _evaluateExpressions(ExpressionPtrVec arguments);
  std::shared_ptr<Object>
  _evaluateArrayIndexExpression(std::shared_ptr<Object> array, std::shared_ptr<Object> index);
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
