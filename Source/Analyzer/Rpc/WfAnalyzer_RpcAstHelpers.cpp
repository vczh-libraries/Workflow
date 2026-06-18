#include "WfAnalyzer_RpcAstHelpers.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection::description;

			namespace rpc_generating
			{
				void SplitTypeFullName(const WString& typeFullName, List<WString>& fragments)
				{
					vint start = 0;
					while (start <= typeFullName.Length())
					{
						vint separator = -1;
						for (vint i = start; i + 1 < typeFullName.Length(); i++)
						{
							if (typeFullName[i] == L':' && typeFullName[i + 1] == L':')
							{
								separator = i;
								break;
							}
						}
						if (separator == -1)
						{
							fragments.Add(typeFullName.Sub(start, typeFullName.Length() - start));
							break;
						}

						fragments.Add(typeFullName.Sub(start, separator - start));
						start = separator + 2;
					}
				}

				Ptr<WfType> CopyType(WfType* type)
				{
					return vl::workflow::analyzer::CopyType(type);
				}

				Ptr<WfType> CreatePredefinedType(WfPredefinedTypeName name)
				{
					auto type = Ptr(new WfPredefinedType);
					type->name = name;
					return type;
				}

				Ptr<WfType> CreateQualifiedType(const WString& fullName)
				{
					List<WString> fragments;
					SplitTypeFullName(fullName, fragments);
					if (fragments.Count() == 0)
					{
						return nullptr;
					}

					Ptr<WfType> type;
					for (vint i = 0; i < fragments.Count(); i++)
					{
						if (i == 0)
						{
							auto top = Ptr(new WfReferenceType);
							top->name.value = fragments[i];
							type = top;
						}
						else
						{
							auto child = Ptr(new WfChildType);
							child->parent = type;
							child->name.value = fragments[i];
							type = child;
						}
					}
					return type;
				}

				Ptr<WfType> CreateTopQualifiedType(const WString& fullName)
				{
					List<WString> fragments;
					SplitTypeFullName(fullName, fragments);
					if (fragments.Count() == 0)
					{
						return nullptr;
					}

					Ptr<WfType> type;
					for (vint i = 0; i < fragments.Count(); i++)
					{
						if (i == 0)
						{
							auto top = Ptr(new WfTopQualifiedType);
							top->name.value = fragments[i];
							type = top;
						}
						else
						{
							auto child = Ptr(new WfChildType);
							child->parent = type;
							child->name.value = fragments[i];
							type = child;
						}
					}
					return type;
				}

				Ptr<WfType> CreateSharedType(const WString& fullName)
				{
					auto type = Ptr(new WfSharedPointerType);
					type->element = CreateQualifiedType(fullName);
					return type;
				}

				Ptr<WfType> CreateRawType(const WString& fullName)
				{
					auto type = Ptr(new WfRawPointerType);
					type->element = CreateQualifiedType(fullName);
					return type;
				}

				Ptr<WfType> CreateNullableType(const WString& fullName)
				{
					auto type = Ptr(new WfNullableType);
					type->element = CreateQualifiedType(fullName);
					return type;
				}

				Nullable<WfPredefinedTypeName> GetPredefinedTypeFromSystemName(const WString& name)
				{
					if (name == L"Boolean") return WfPredefinedTypeName::Bool;
					if (name == (sizeof(vint) == sizeof(vint64_t) ? L"Int64" : L"Int32")) return WfPredefinedTypeName::Int;
					if (name == L"String") return WfPredefinedTypeName::String;
					if (name == L"Object") return WfPredefinedTypeName::Object;
					if (name == L"Void") return WfPredefinedTypeName::Void;
					return {};
				}

				Ptr<WfType> NormalizeRpcGeneratedType(Ptr<WfType> type)
				{
					if (!type)
					{
						return nullptr;
					}

					if (auto child = type.Cast<WfChildType>())
					{
						child->parent = NormalizeRpcGeneratedType(child->parent);
						if (auto reference = child->parent.Cast<WfReferenceType>())
						{
							if (reference->name.value == L"system")
							{
								if (auto predefined = GetPredefinedTypeFromSystemName(child->name.value))
								{
									return CreatePredefinedType(predefined.Value());
								}
							}
						}
					}
					else if (auto top = type.Cast<WfTopQualifiedType>())
					{
						auto reference = Ptr(new WfReferenceType);
						reference->name = top->name;
						return reference;
					}
					else if (auto raw = type.Cast<WfRawPointerType>())
					{
						raw->element = NormalizeRpcGeneratedType(raw->element);
					}
					else if (auto shared = type.Cast<WfSharedPointerType>())
					{
						shared->element = NormalizeRpcGeneratedType(shared->element);
					}
					else if (auto nullable = type.Cast<WfNullableType>())
					{
						nullable->element = NormalizeRpcGeneratedType(nullable->element);
					}
					else if (auto enumerable = type.Cast<WfEnumerableType>())
					{
						enumerable->element = NormalizeRpcGeneratedType(enumerable->element);
					}
					else if (auto map = type.Cast<WfMapType>())
					{
						if (map->key)
						{
							map->key = NormalizeRpcGeneratedType(map->key);
						}
						map->value = NormalizeRpcGeneratedType(map->value);
					}
					else if (auto observable = type.Cast<WfObservableListType>())
					{
						observable->element = NormalizeRpcGeneratedType(observable->element);
					}
					else if (auto function = type.Cast<WfFunctionType>())
					{
						function->result = NormalizeRpcGeneratedType(function->result);
						for (vint i = 0; i < function->arguments.Count(); i++)
						{
							function->arguments[i] = NormalizeRpcGeneratedType(function->arguments[i]);
						}
					}
					return type;
				}

				Ptr<WfExpression> CreateNull()
				{
					auto expression = Ptr(new WfLiteralExpression);
					expression->value = WfLiteralValue::Null;
					return expression;
				}

				Ptr<WfExpression> CreateIsNull(Ptr<WfExpression> expression)
				{
					auto testing = Ptr(new WfTypeTestingExpression);
					testing->test = WfTypeTesting::IsNull;
					testing->expression = expression;
					return testing;
				}

				Ptr<WfExpression> CreateIsNotNull(Ptr<WfExpression> expression)
				{
					auto testing = Ptr(new WfTypeTestingExpression);
					testing->test = WfTypeTesting::IsNotNull;
					testing->expression = expression;
					return testing;
				}

				Ptr<WfExpression> CreateIsType(Ptr<WfExpression> expression, Ptr<WfType> type)
				{
					auto testing = Ptr(new WfTypeTestingExpression);
					testing->test = WfTypeTesting::IsType;
					testing->expression = expression;
					testing->type = type;
					return testing;
				}

				Ptr<WfExpression> CreateBool(bool value)
				{
					auto expression = Ptr(new WfLiteralExpression);
					expression->value = value ? WfLiteralValue::True : WfLiteralValue::False;
					return expression;
				}

				Ptr<WfExpression> CreateInt(vint value)
				{
					auto expression = Ptr(new WfIntegerExpression);
					expression->value.value = itow(value);
					return expression;
				}

				Ptr<WfExpression> CreateString(const WString& value)
				{
					auto expression = Ptr(new WfStringExpression);
					expression->value.value = value;
					return expression;
				}

				Ptr<WfExpression> CreateReference(const WString& name)
				{
					auto expression = Ptr(new WfReferenceExpression);
					expression->name.value = name;
					return expression;
				}

				Ptr<WfExpression> CreateThis()
				{
					return Ptr(new WfThisExpression);
				}

				Ptr<WfExpression> CreateQualifiedExpression(const WString& fullName)
				{
					List<WString> fragments;
					SplitTypeFullName(fullName, fragments);
					if (fragments.Count() == 0)
					{
						return nullptr;
					}

					Ptr<WfExpression> expression = CreateReference(fragments[0]);
					for (vint i = 1; i < fragments.Count(); i++)
					{
						auto child = Ptr(new WfChildExpression);
						child->parent = expression;
						child->name.value = fragments[i];
						expression = child;
					}
					return expression;
				}

				Ptr<WfExpression> CreateMember(Ptr<WfExpression> parent, const WString& name)
				{
					auto expression = Ptr(new WfMemberExpression);
					expression->parent = parent;
					expression->name.value = name;
					return expression;
				}

				Ptr<WfExpression> CreateBinary(WfBinaryOperator op, Ptr<WfExpression> first, Ptr<WfExpression> second)
				{
					auto expression = Ptr(new WfBinaryExpression);
					expression->op = op;
					expression->first = first;
					expression->second = second;
					return expression;
				}

				Ptr<WfExpression> CreateUnary(WfUnaryOperator op, Ptr<WfExpression> operand)
				{
					auto expression = Ptr(new WfUnaryExpression);
					expression->op = op;
					expression->operand = operand;
					return expression;
				}

				Ptr<WfExpression> CreateAssign(Ptr<WfExpression> left, Ptr<WfExpression> right)
				{
					return CreateBinary(WfBinaryOperator::Assign, left, right);
				}

				Ptr<WfExpression> CreateIndex(Ptr<WfExpression> collection, Ptr<WfExpression> index)
				{
					return CreateBinary(WfBinaryOperator::Index, collection, index);
				}

				Ptr<WfExpression> CreateCast(Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto cast = Ptr(new WfTypeCastingExpression);
					cast->strategy = WfTypeCastingStrategy::Strong;
					cast->type = type;
					cast->expression = expression;
					return cast;
				}

				Ptr<WfExpression> CreateWeakCast(Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto cast = Ptr(new WfTypeCastingExpression);
					cast->strategy = WfTypeCastingStrategy::Weak;
					cast->type = type;
					cast->expression = expression;
					return cast;
				}

				Ptr<WfExpression> CreateInfer(Ptr<WfExpression> expression, Ptr<WfType> type)
				{
					auto infer = Ptr(new WfInferExpression);
					infer->expression = expression;
					infer->type = type;
					return infer;
				}

				Ptr<WfConstructorArgument> CreateConstructorArgument(Ptr<WfExpression> key, Ptr<WfExpression> value)
				{
					auto argument = Ptr(new WfConstructorArgument);
					argument->key = key;
					argument->value = value;
					return argument;
				}

				Ptr<WfConstructorExpression> CreateConstructor()
				{
					return Ptr(new WfConstructorExpression);
				}

				Ptr<WfExpression> CreateRpcExceptionExpression(Ptr<WfExpression> message)
				{
					auto constructor = CreateConstructor();
					constructor->arguments.Add(CreateConstructorArgument(CreateReference(L"message"), message));
					return CreateInfer(constructor, CreateTypeFromCpp<rpc_controller::RpcException>());
				}

				Ptr<WfType> CreateRpcEventExceptionMapType()
				{
					return CreateTypeFromCpp<Dictionary<vint, rpc_controller::RpcException>>();
				}

				Ptr<WfExpression> CreateNewClass(Ptr<WfType> type)
				{
					auto expression = Ptr(new WfNewClassExpression);
					expression->type = type;
					return expression;
				}

				Ptr<WfExpression> CreateNewInterface(Ptr<WfType> type)
				{
					auto expression = Ptr(new WfNewInterfaceExpression);
					expression->type = type;
					return expression;
				}

				Ptr<WfStatement> CreateExpressionStatement(Ptr<WfExpression> expression)
				{
					auto statement = Ptr(new WfExpressionStatement);
					statement->expression = expression;
					return statement;
				}

				Ptr<WfStatement> CreateReturn(Ptr<WfExpression> expression)
				{
					auto statement = Ptr(new WfReturnStatement);
					statement->expression = expression;
					return statement;
				}

				Ptr<WfStatement> CreateRaise(const WString& message)
				{
					auto statement = Ptr(new WfRaiseExceptionStatement);
					statement->expression = CreateString(message);
					return statement;
				}

				Ptr<WfStatement> CreateRaise(Ptr<WfExpression> expression)
				{
					auto statement = Ptr(new WfRaiseExceptionStatement);
					statement->expression = expression;
					return statement;
				}

				Ptr<WfVariableDeclaration> CreateVariableDeclaration(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto declaration = Ptr(new WfVariableDeclaration);
					declaration->name.value = name;
					declaration->type = type;
					declaration->expression = expression;
					return declaration;
				}

				Ptr<WfStatement> CreateVariableStatement(const WString& name, Ptr<WfType> type, Ptr<WfExpression> expression)
				{
					auto statement = Ptr(new WfVariableStatement);
					statement->variable = CreateVariableDeclaration(name, type, expression);
					return statement;
				}

				Ptr<WfStatement> CreateInferredVariableStatement(const WString& name, Ptr<WfExpression> expression)
				{
					return CreateVariableStatement(name, nullptr, expression);
				}

				Ptr<WfStatement> CreateIf(Ptr<WfExpression> condition, Ptr<WfStatement> trueBranch, Ptr<WfStatement> falseBranch)
				{
					auto statement = Ptr(new WfIfStatement);
					statement->expression = condition;
					statement->trueBranch = trueBranch;
					statement->falseBranch = falseBranch;
					return statement;
				}

				Ptr<WfStatement> CreateTry(Ptr<WfStatement> protectedStatement, const WString& name, Ptr<WfStatement> catchStatement, Ptr<WfStatement> finallyStatement)
				{
					auto statement = Ptr(new WfTryStatement);
					statement->protectedStatement = protectedStatement;
					statement->name.value = name;
					statement->catchStatement = catchStatement;
					statement->finallyStatement = finallyStatement;
					return statement;
				}

				Ptr<WfStatement> CreateTryCatch(Ptr<WfStatement> protectedStatement, const WString& name, Ptr<WfStatement> catchStatement)
				{
					return CreateTry(protectedStatement, name, catchStatement, nullptr);
				}

				Ptr<WfForEachStatement> CreateForEach(const WString& name, Ptr<WfExpression> collection, Ptr<WfStatement> body)
				{
					auto statement = Ptr(new WfForEachStatement);
					statement->name.value = name;
					statement->direction = WfForEachDirection::Normal;
					statement->collection = collection;
					statement->statement = body;
					return statement;
				}

				Ptr<WfStatement> CreateWhile(Ptr<WfExpression> condition, Ptr<WfStatement> body)
				{
					auto statement = Ptr(new WfWhileStatement);
					statement->condition = condition;
					statement->statement = body;
					return statement;
				}

				Ptr<WfBlockStatement> CreateBlock()
				{
					return Ptr(new WfBlockStatement);
				}

				void AddStatement(Ptr<WfBlockStatement> block, Ptr<WfStatement> statement)
				{
					block->statements.Add(statement);
				}

				void AddRpcMethodExceptionRaise(Ptr<WfBlockStatement> block, Ptr<WfExpression> value)
				{
					AddStatement(
						block,
						CreateExpressionStatement(CreateCall(
							CreateQualifiedExpression(L"system::IRpcLifecycle::ReadMethodException"),
							value)));
				}

				void AddRpcEventExceptionMapSet(Ptr<WfBlockStatement> block, const WString& mapName, Ptr<WfExpression> clientId, Ptr<WfExpression> message)
				{
					AddStatement(block, CreateExpressionStatement(CreateCall(
						CreateMember(CreateReference(mapName), L"Set"),
						clientId,
						CreateRpcExceptionExpression(message))));
				}

				void AddRpcEventExceptionRaise(Ptr<WfBlockStatement> block, Ptr<WfExpression> value)
				{
					AddStatement(
						block,
						CreateExpressionStatement(CreateCall(
							CreateQualifiedExpression(L"system::IRpcLifecycle::ReadEventException"),
							value)));
				}

				Ptr<WfFunctionArgument> CreateFunctionArgument(const WString& name, Ptr<WfType> type)
				{
					auto argument = Ptr(new WfFunctionArgument);
					argument->name.value = name;
					argument->type = type;
					return argument;
				}

				Ptr<WfFunctionDeclaration> CreateFunctionDeclaration(const WString& name, Ptr<WfType> returnType, WfFunctionKind kind, WfFunctionAnonymity anonymity)
				{
					auto declaration = Ptr(new WfFunctionDeclaration);
					declaration->name.value = name;
					declaration->returnType = returnType;
					declaration->functionKind = kind;
					declaration->anonymity = anonymity;
					declaration->statement = CreateBlock();
					return declaration;
				}

				Ptr<WfExpression> CreateFunctionExpression(Ptr<WfFunctionDeclaration> declaration)
				{
					auto expression = Ptr(new WfFunctionExpression);
					expression->function = declaration;
					return expression;
				}

				Ptr<WfClassDeclaration> CreateClassDeclaration(const WString& name)
				{
					auto declaration = Ptr(new WfClassDeclaration);
					declaration->name.value = name;
					declaration->kind = WfClassKind::Class;
					declaration->constructorType = WfConstructorType::Undefined;
					return declaration;
				}

				Ptr<WfConstructorDeclaration> CreateConstructorDeclaration(WfConstructorType constructorType)
				{
					auto declaration = Ptr(new WfConstructorDeclaration);
					declaration->constructorType = constructorType;
					declaration->statement = CreateBlock();
					return declaration;
				}

				void AddSwitchCase(Ptr<WfSwitchStatement> switchStat, Ptr<WfExpression> expression, Ptr<WfStatement> statement)
				{
					auto switchCase = Ptr(new WfSwitchCase);
					switchCase->expression = expression;
					switchCase->statement = statement;
					switchStat->caseBranches.Add(switchCase);
				}
			}
		}
	}
}
