/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Analyzer::RPC AST Helpers

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_ANALYZER_RPC_WFANALYZER_RPCASTHELPERS
#define VCZH_WORKFLOW_ANALYZER_RPC_WFANALYZER_RPCASTHELPERS

#include "../WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			namespace rpc_generating
			{
				extern void														SplitTypeFullName(const WString& typeFullName, collections::List<WString>& fragments);
				extern Ptr<WfType>												CopyType(WfType* type);
				extern Ptr<WfType>												CreatePredefinedType(WfPredefinedTypeName name);
				extern Ptr<WfType>												CreateQualifiedType(const WString& fullName);
				extern Ptr<WfType>												CreateTopQualifiedType(const WString& fullName);
				extern Ptr<WfType>												CreateSharedType(const WString& fullName);
				extern Ptr<WfType>												CreateRawType(const WString& fullName);
				extern Ptr<WfType>												CreateNullableType(const WString& fullName);
				extern Ptr<WfType>												NormalizeRpcGeneratedType(Ptr<WfType> type);
				extern Ptr<WfExpression>										CreateNull();
				extern Ptr<WfExpression>										CreateIsNull(Ptr<WfExpression> expression);
				extern Ptr<WfExpression>										CreateIsNotNull(Ptr<WfExpression> expression);
				extern Ptr<WfExpression>										CreateIsType(Ptr<WfExpression> expression, Ptr<WfType> type);
				extern Ptr<WfExpression>										CreateBool(bool value);
				extern Ptr<WfExpression>										CreateInt(vint value);
				extern Ptr<WfExpression>										CreateString(const WString& value);
				extern Ptr<WfExpression>										CreateReference(const WString& name);
				extern Ptr<WfExpression>										CreateThis();
				extern Ptr<WfExpression>										CreateQualifiedExpression(const WString& fullName);
				extern Ptr<WfExpression>										CreateMember(Ptr<WfExpression> parent, const WString& name);
				extern Ptr<WfExpression>										CreateBinary(WfBinaryOperator op, Ptr<WfExpression> first, Ptr<WfExpression> second);
				extern Ptr<WfExpression>										CreateUnary(WfUnaryOperator op, Ptr<WfExpression> operand);
				extern Ptr<WfExpression>										CreateAssign(Ptr<WfExpression> left, Ptr<WfExpression> right);
				extern Ptr<WfExpression>										CreateIndex(Ptr<WfExpression> collection, Ptr<WfExpression> index);
				extern Ptr<WfExpression>										CreateCast(Ptr<WfType> type, Ptr<WfExpression> expression);
				extern Ptr<WfExpression>										CreateWeakCast(Ptr<WfType> type, Ptr<WfExpression> expression);
				extern Ptr<WfExpression>										CreateInfer(Ptr<WfExpression> expression, Ptr<WfType> type);
				extern Ptr<WfConstructorArgument>								CreateConstructorArgument(Ptr<WfExpression> key, Ptr<WfExpression> value);
				extern Ptr<WfConstructorExpression>								CreateConstructor();
				extern Ptr<WfExpression>										CreateRpcExceptionExpression(Ptr<WfExpression> message);
				extern Ptr<WfType>												CreateRpcEventExceptionMapType();
				extern Ptr<WfExpression>										CreateNewClass(Ptr<WfType> type);
				extern Ptr<WfExpression>										CreateNewInterface(Ptr<WfType> type);
				extern Ptr<WfStatement>											CreateExpressionStatement(Ptr<WfExpression> expression);
				extern Ptr<WfStatement>											CreateReturn(Ptr<WfExpression> expression);
				extern Ptr<WfStatement>											CreateRaise(const WString& message);
				extern Ptr<WfStatement>											CreateRaise(Ptr<WfExpression> expression);
				extern Ptr<WfVariableDeclaration>								CreateVariableDeclaration(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression);
				extern Ptr<WfStatement>											CreateVariableStatement(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression);
				extern Ptr<WfStatement>											CreateInferredVariableStatement(const WString& name, Ptr<WfExpression> expression);
				extern Ptr<WfStatement>											CreateIf(Ptr<WfExpression> condition, Ptr<WfStatement> trueBranch, Ptr<WfStatement> falseBranch = nullptr);
				extern Ptr<WfStatement>											CreateTry(Ptr<WfStatement> protectedStatement, const WString& name, Ptr<WfStatement> catchStatement, Ptr<WfStatement> finallyStatement);
				extern Ptr<WfStatement>											CreateTryCatch(Ptr<WfStatement> protectedStatement, const WString& name, Ptr<WfStatement> catchStatement);
				extern Ptr<WfForEachStatement>									CreateForEach(const WString& name, Ptr<WfExpression> collection, Ptr<WfStatement> body);
				extern Ptr<WfStatement>											CreateWhile(Ptr<WfExpression> condition, Ptr<WfStatement> body);
				extern Ptr<WfBlockStatement>									CreateBlock();
				extern void														AddStatement(Ptr<WfBlockStatement> block, Ptr<WfStatement> statement);
				extern void														AddRpcMethodExceptionRaise(Ptr<WfBlockStatement> block, Ptr<WfExpression> value);
				extern void														AddRpcEventExceptionMapSet(Ptr<WfBlockStatement> block, const WString& mapName, Ptr<WfExpression> clientId, Ptr<WfExpression> message);
				extern void														AddRpcEventExceptionRaise(Ptr<WfBlockStatement> block, Ptr<WfExpression> value);
				extern Ptr<WfFunctionArgument>									CreateFunctionArgument(const WString& name, Ptr<WfType> type);
				extern Ptr<WfFunctionDeclaration>								CreateFunctionDeclaration(const WString& name, Ptr<WfType> returnType, WfFunctionKind kind, WfFunctionAnonymity anonymity = WfFunctionAnonymity::Named);
				extern Ptr<WfExpression>										CreateFunctionExpression(Ptr<WfFunctionDeclaration> declaration);
				extern Ptr<WfClassDeclaration>									CreateClassDeclaration(const WString& name);
				extern Ptr<WfConstructorDeclaration>								CreateConstructorDeclaration(WfConstructorType constructorType = WfConstructorType::SharedPtr);
				extern void														AddSwitchCase(Ptr<WfSwitchStatement> switchStat, Ptr<WfExpression> expression, Ptr<WfStatement> statement);

				template<typename T>
				Ptr<WfType> CreateTypeFromCpp()
				{
					return NormalizeRpcGeneratedType(GetTypeFromTypeInfo(reflection::description::TypeInfoRetriver<T>::CreateTypeInfo().Obj()));
				}

				template<typename ...TArgs>
				Ptr<WfCallExpression> CreateCall(Ptr<WfExpression> function, TArgs... arguments)
				{
					auto expression = Ptr(new WfCallExpression);
					expression->function = function;
					if constexpr (sizeof...(arguments) > 0)
					{
						(expression->arguments.Add(arguments), ...);
					}
					return expression;
				}
			}
		}
	}
}

#endif
