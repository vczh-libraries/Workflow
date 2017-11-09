#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection::description;

/***********************************************************************
SetCodeRange
***********************************************************************/

			class SetCodeRangeVisitor : public traverse_visitor::ModuleVisitor
			{
			public:
				ParsingTextRange						range;
				bool									asOffset;

				SetCodeRangeVisitor(ParsingTextRange _range, bool _asOffset)
					:range(_range)
					, asOffset(_asOffset)
				{
				}

				void UpdateTextPos(ParsingTextPos& pos)
				{
					if (pos.row == -1 || pos.column == -1)
					{
						pos = range.start;
					}
					else 
					{
						if (pos.row == 0)
						{
							pos.column += range.start.column;
						}
						pos.row += range.start.row;
					}
				}

				void Traverse(ParsingTreeCustomBase* node)override
				{
					if (node->codeRange == ParsingTextRange())
					{
						node->codeRange = range;
					}
					else if (asOffset)
					{
						UpdateTextPos(node->codeRange.start);
						UpdateTextPos(node->codeRange.end);
						if (node->codeRange.codeIndex == -1)
						{
							node->codeRange.codeIndex = range.codeIndex;
						}
					}
				}

				void Traverse(ParsingToken& token)override
				{
					if (token.codeRange == ParsingTextRange())
					{
						token.codeRange = range;
					}
					else if (asOffset)
					{
						UpdateTextPos(token.codeRange.start);
						UpdateTextPos(token.codeRange.end);
						if (token.codeRange.codeIndex == -1)
						{
							token.codeRange.codeIndex = range.codeIndex;
						}
					}
				}
			};

			void SetCodeRange(Ptr<WfType> node, parsing::ParsingTextRange codeRange, bool asOffset)
			{
				SetCodeRangeVisitor(codeRange, asOffset).VisitField(node.Obj());
			}

			void SetCodeRange(Ptr<WfExpression> node, parsing::ParsingTextRange codeRange, bool asOffset)
			{
				SetCodeRangeVisitor(codeRange, asOffset).VisitField(node.Obj());
			}

			void SetCodeRange(Ptr<WfStatement> node, parsing::ParsingTextRange codeRange, bool asOffset)
			{
				SetCodeRangeVisitor(codeRange, asOffset).VisitField(node.Obj());
			}

			void SetCodeRange(Ptr<WfDeclaration> node, parsing::ParsingTextRange codeRange, bool asOffset)
			{
				SetCodeRangeVisitor(codeRange, asOffset).VisitField(node.Obj());
			}

			void SetCodeRange(Ptr<WfModule> node, parsing::ParsingTextRange codeRange, bool asOffset)
			{
				SetCodeRangeVisitor(codeRange, asOffset).VisitField(node.Obj());
			}

/***********************************************************************
ContextFreeModuleDesugar
***********************************************************************/

			class ContextFreeDesugarVisitor : public traverse_visitor::ModuleVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				WfClassDeclaration*						surroundingClassDecl = nullptr;
				WfNewInterfaceExpression*				surroundingLambda = nullptr;

				ContextFreeDesugarVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Traverse(WfFormatExpression* node)override
				{
					if (node->expandedExpression)
					{
						return;
					}

					List<Ptr<WfExpression>> expressions;
					const wchar_t* reading = node->value.value.Buffer();

					const wchar_t* textPosCounter = reading;
					ParsingTextPos formatPos = node->codeRange.start;
					formatPos.column += 2;

					while (*reading)
					{
						const wchar_t* begin = wcsstr(reading, L"$(");
						if (begin)
						{
							Ptr<WfStringExpression> expression = new WfStringExpression;
							expression->codeRange = node->codeRange;
							expression->value.value = WString(reading, vint(begin - reading));
							expressions.Add(expression);
						}
						else
						{
							break;
						}

						const wchar_t* end = begin + 2;
						vint counter = 1;
						while (wchar_t c = *end++)
						{
							switch (c)
							{
							case L'(':
								counter++;
								break;
							case L')':
								counter--;
								break;
							}
							if (counter == 0)
							{
								break;
							}
						}

						if (counter != 0)
						{
							auto error = WfErrors::WrongFormatStringSyntax(node);
							error->errorMessage += L" (Does not find matched close bracket.)";
							manager->errors.Add(error);
							return;
						}
						else
						{
							WString input(begin + 2, vint(end - begin - 3));
							List<Ptr<ParsingError>> errors;
							if (auto expression = WfParseExpression(input, manager->parsingTable, errors))
							{
								expressions.Add(expression);

								while (textPosCounter++ < begin + 2)
								{
									switch (textPosCounter[-1])
									{
									case '\n':
										formatPos.row++;
										formatPos.column = 0;
										break;
									default:
										formatPos.column++;
										break;
									}
								}
								SetCodeRange(expression, { formatPos,formatPos,node->codeRange.codeIndex }, true);
							}
							FOREACH(Ptr<ParsingError>, originalError, errors)
							{
								auto error = WfErrors::WrongFormatStringSyntax(node);
								error->errorMessage += L" (" + originalError->errorMessage + L")";
								manager->errors.Add(error);
							}
							reading = end;
						}
					}
					if (*reading || expressions.Count() == 0)
					{
						Ptr<WfStringExpression> expression = new WfStringExpression;
						expression->codeRange = node->codeRange;
						expression->value.value = reading;
						expressions.Add(expression);
					}

					if (expressions.Count() > 0)
					{
						Ptr<WfExpression> current = expressions[0];
						FOREACH(Ptr<WfExpression>, expression, From(expressions).Skip(1))
						{
							Ptr<WfBinaryExpression> binary = new WfBinaryExpression;
							binary->codeRange = node->codeRange;
							binary->first = current;
							binary->second = expression;
							binary->op = WfBinaryOperator::Union;
							current = binary;
						}

						node->expandedExpression = current;
						SetCodeRange(node->expandedExpression, node->codeRange);
					}
				}

				void Traverse(WfAutoPropertyDeclaration* node)override
				{
					if (node->expandedDeclarations.Count() > 0)
					{
						return;
					}

					bool needVariable = false;
					bool needVirtual = false;
					bool needEvent = false;
					bool needProperty = false;

					if (surroundingClassDecl)
					{
						switch (surroundingClassDecl->kind)
						{
						case WfClassKind::Class:
							{
								needVariable = true;
								needVirtual = false;
								needEvent = node->configObserve == WfAPObserve::Observable;
								needProperty = true;
							}
							break;
						case WfClassKind::Interface:
							{
								needVariable = false;
								needVirtual = true;
								needEvent = node->configObserve == WfAPObserve::Observable;
								needProperty = true;
							}
							break;
						}
					}
					else if (surroundingLambda)
					{
						needVariable = true;
						needVirtual = false;
						needEvent = false;
						needProperty = false;
					}

					auto varName = L"<prop>" + node->name.value;
					auto getterName = L"Get" + node->name.value;
					auto setterName = L"Set" + node->name.value;
					auto eventName = node->name.value + L"Changed";

					if (needVariable && node->expression)
					{
						auto decl = MakePtr<WfVariableDeclaration>();
						node->expandedDeclarations.Add(decl);

						decl->name.value = varName;
						decl->type = CopyType(node->type);
						decl->expression = CopyExpression(node->expression, true);

						auto att = MakePtr<WfAttribute>();
						att->category.value = L"cpp";
						att->name.value = L"Private";
						decl->attributes.Add(att);
					}

					{
						auto decl = MakePtr<WfFunctionDeclaration>();
						node->expandedDeclarations.Add(decl);

						decl->anonymity = WfFunctionAnonymity::Named;
						decl->name.value = getterName;
						decl->returnType = CopyType(node->type);

						if (!needVirtual)
						{
							auto refExpr = MakePtr<WfReferenceExpression>();
							refExpr->name.value = varName;

							auto returnStat = MakePtr<WfReturnStatement>();
							returnStat->expression = refExpr;

							auto block = MakePtr<WfBlockStatement>();
							block->statements.Add(returnStat);
							decl->statement = block;
						}
					}

					if (!needVirtual || node->configConst == WfAPConst::Writable)
					{
						auto decl = MakePtr<WfFunctionDeclaration>();
						node->expandedDeclarations.Add(decl);

						decl->anonymity = WfFunctionAnonymity::Named;
						decl->name.value = setterName;

						if (node->configConst == WfAPConst::Readonly)
						{
							auto att = MakePtr<WfAttribute>();
							att->category.value = L"cpp";
							att->name.value = L"Protected";
							decl->attributes.Add(att);
						}

						{
							auto argument = MakePtr<WfFunctionArgument>();
							argument->name.value = L"<value>";
							argument->type = CopyType(node->type);
							decl->arguments.Add(argument);
						}
						{
							auto voidType = MakePtr<WfPredefinedType>();
							voidType->name = WfPredefinedTypeName::Void;
							decl->returnType = voidType;
						}

						if (!needVirtual)
						{
							auto block = MakePtr<WfBlockStatement>();
							decl->statement = block;

							auto createBinaryExpr = [&](WfBinaryOperator op)
							{
								auto refArgument = MakePtr<WfReferenceExpression>();
								refArgument->name.value = L"<value>";

								auto refVar = MakePtr<WfReferenceExpression>();
								refVar->name.value = varName;

								auto binaryExpr = MakePtr<WfBinaryExpression>();
								binaryExpr->first = refVar;
								binaryExpr->second = refArgument;
								binaryExpr->op = op;

								return binaryExpr;
							};

							if (node->configObserve == WfAPObserve::Observable)
							{
								auto ifStat = MakePtr<WfIfStatement>();
								ifStat->expression = createBinaryExpr(WfBinaryOperator::NE);

								auto trueBlock = MakePtr<WfBlockStatement>();
								ifStat->trueBranch = trueBlock;

								{
									auto stat = MakePtr<WfExpressionStatement>();
									stat->expression = createBinaryExpr(WfBinaryOperator::Assign);
									trueBlock->statements.Add(stat);
								}
								{
									auto refEvent = MakePtr<WfReferenceExpression>();
									refEvent->name.value = eventName;

									auto callExpr = MakePtr<WfCallExpression>();
									callExpr->function = refEvent;

									auto stat = MakePtr<WfExpressionStatement>();
									stat->expression = callExpr;
									trueBlock->statements.Add(stat);
								}

								block->statements.Add(ifStat);
							}
							else
							{
								auto stat = MakePtr<WfExpressionStatement>();
								stat->expression = createBinaryExpr(WfBinaryOperator::Assign);

								block->statements.Add(stat);
							}
						}
					}

					if (needEvent)
					{
						auto decl = MakePtr<WfEventDeclaration>();
						node->expandedDeclarations.Add(decl);

						decl->name.value = eventName;
					}

					if (needProperty)
					{
						auto decl = MakePtr<WfPropertyDeclaration>();
						node->expandedDeclarations.Add(decl);

						decl->type = CopyType(node->type);
						decl->name.value = node->name.value;
						decl->getter.value = getterName;
						if (node->configConst == WfAPConst::Writable)
						{
							decl->setter.value = setterName;
						}
						if (node->configObserve == WfAPObserve::Observable)
						{
							decl->valueChangedEvent.value = eventName;
						}
					}

					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						auto classMember = MakePtr<WfClassMember>();
						decl->classMember = classMember;

						classMember->kind = WfClassMemberKind::Normal;
						if (surroundingLambda)
						{
							if (decl->name.value == getterName)
							{
								classMember->kind = WfClassMemberKind::Override;
							}
							else if (decl->name.value == setterName)
							{
								if (node->configConst == WfAPConst::Writable)
								{
									classMember->kind = WfClassMemberKind::Override;
								}
							}
						}

						SetCodeRange(decl, node->codeRange);
					}
				}

				void Traverse(WfCastResultInterfaceDeclaration* node)override
				{
					if (node->expandedDeclarations.Count() > 0)
					{
						return;
					}

					auto decl = MakePtr<WfClassDeclaration>();
					node->expandedDeclarations.Add(decl);
					decl->kind = WfClassKind::Interface;
					decl->constructorType = WfConstructorType::SharedPtr;
					decl->name.value = node->name.value;
					decl->baseTypes.Add(CopyType(node->baseType));

					{
						auto funcDecl = MakePtr<WfFunctionDeclaration>();
						decl->declarations.Add(funcDecl);
						{
							funcDecl->classMember = MakePtr<WfClassMember>();
							funcDecl->classMember->kind = WfClassMemberKind::Static;
						}
						funcDecl->anonymity = WfFunctionAnonymity::Named;
						funcDecl->name.value = L"CastResult";
						funcDecl->returnType = CopyType(node->elementType);
						{
							auto argument = MakePtr<WfFunctionArgument>();
							funcDecl->arguments.Add(argument);
							argument->name.value = L"value";
							argument->type = GetTypeFromTypeInfo(TypeInfoRetriver<Value>::CreateTypeInfo().Obj());
						}
						{
							auto block = MakePtr<WfBlockStatement>();
							funcDecl->statement = block;

							auto refValue = MakePtr<WfReferenceExpression>();
							refValue->name.value = L"value";

							auto castExpr = MakePtr<WfTypeCastingExpression>();
							castExpr->strategy = WfTypeCastingStrategy::Strong;
							castExpr->type = CopyType(node->elementType);
							castExpr->expression = refValue;

							auto stat = MakePtr<WfReturnStatement>();
							stat->expression = castExpr;

							block->statements.Add(stat);
						}
					}
					{
						auto funcDecl = MakePtr<WfFunctionDeclaration>();
						decl->declarations.Add(funcDecl);
						{
							funcDecl->classMember = MakePtr<WfClassMember>();
							funcDecl->classMember->kind = WfClassMemberKind::Static;
						}
						funcDecl->anonymity = WfFunctionAnonymity::Named;
						funcDecl->name.value = L"StoreResult";
						funcDecl->returnType = GetTypeFromTypeInfo(TypeInfoRetriver<Value>::CreateTypeInfo().Obj());
						{
							auto argument = MakePtr<WfFunctionArgument>();
							funcDecl->arguments.Add(argument);
							argument->name.value = L"value";
							argument->type = CopyType(node->elementType);
						}
						{
							auto block = MakePtr<WfBlockStatement>();
							funcDecl->statement = block;

							auto refValue = MakePtr<WfReferenceExpression>();
							refValue->name.value = L"value";

							auto stat = MakePtr<WfReturnStatement>();
							stat->expression = refValue;

							block->statements.Add(stat);
						}
					}

					SetCodeRange(Ptr<WfDeclaration>(decl), node->codeRange);
				}

#define PUSH_SURROUNDING\
					auto o1 = surroundingClassDecl;\
					auto o2 = surroundingLambda;\
					surroundingClassDecl = nullptr;\
					surroundingLambda = nullptr;\

#define POP_SURROUNDING\
					surroundingClassDecl = o1;\
					surroundingLambda = o2;\

				void Visit(WfClassDeclaration* node)override
				{
					PUSH_SURROUNDING
					surroundingClassDecl = node;
					traverse_visitor::DeclarationVisitor::Visit(node);
					POP_SURROUNDING
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					PUSH_SURROUNDING
					surroundingLambda = node;
					traverse_visitor::ExpressionVisitor::Visit(node);
					POP_SURROUNDING
				}

#undef PUSH_SURROUNDING
#undef POP_SURROUNDING
			};

			void ContextFreeModuleDesugar(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				ContextFreeDesugarVisitor(manager).VisitField(module.Obj());
			}

			void ContextFreeDeclarationDesugar(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration)
			{
				ContextFreeDesugarVisitor(manager).VisitField(declaration.Obj());
			}

			void ContextFreeStatementDesugar(WfLexicalScopeManager* manager, Ptr<WfStatement> statement)
			{
				ContextFreeDesugarVisitor(manager).VisitField(statement.Obj());
			}

			void ContextFreeExpressionDesugar(WfLexicalScopeManager* manager, Ptr<WfExpression> expression)
			{
				ContextFreeDesugarVisitor(manager).VisitField(expression.Obj());
			}
		}
	}
}