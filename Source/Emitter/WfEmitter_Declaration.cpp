#include "WfEmitter.h"

namespace vl
{
	namespace workflow
	{
		namespace emitter
		{
			using namespace collections;
			using namespace reflection::description;
			using namespace analyzer;
			using namespace runtime;
			using namespace typeimpl;

			typedef WfInstruction Ins;

#define INSTRUCTION(X) context.AddInstruction(node, X)

/***********************************************************************
GenerateInstructions(Initialize)
***********************************************************************/

			class GenerateInitializeInstructionsVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCodegenContext&						context;

				GenerateInitializeInstructionsVisitor(WfCodegenContext& _context)
					:context(_context)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					for (auto decl : node->declarations)
					{
						GenerateInitializeInstructions(context, decl);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto scope = context.manager->nodeScopes[node].Obj();
					auto symbol = scope->symbols[node->name.value][0];
					vint variableIndex = context.globalVariables[symbol.Obj()];
					GenerateExpressionInstructions(context, node->expression);
					INSTRUCTION(Ins::StoreGlobalVar(variableIndex));
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
				}

				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
				}

				void Visit(WfEnumDeclaration* node)override
				{
				}

				void Visit(WfStructDeclaration* node)override
				{
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}
			};

			void GenerateInitializeInstructions(WfCodegenContext& context, Ptr<WfDeclaration> declaration)
			{
				GenerateInitializeInstructionsVisitor visitor(context);
				declaration->Accept(&visitor);
			}

/***********************************************************************
GenerateInstructions(Declaration)
***********************************************************************/

			Ptr<WfCodegenFunctionContext> GenerateFunctionInstructions_Prolog(WfCodegenContext& context, WfLexicalScope* scope, Ptr<WfAssemblyFunction> meta, Ptr<ITypeInfo> returnType, Ptr<WfLexicalSymbol> recursiveLambdaSymbol, const List<Ptr<WfLexicalSymbol>>& argumentSymbols, const List<Ptr<WfLexicalSymbol>>& capturedSymbols, glr::ParsingAstBase* node)
			{
				auto functionContext = Ptr(new WfCodegenFunctionContext);
				functionContext->function = meta;
				context.functionContext = functionContext;
				{
					for (auto [argumentSymbol, index] : indexed(argumentSymbols))
					{
						functionContext->arguments.Add(argumentSymbol.Obj(), index);
					}
					for (auto [capturedSymbol, index] : indexed(capturedSymbols))
					{
						functionContext->capturedVariables.Add(capturedSymbol.Obj(), index);
					}
				}
				if (recursiveLambdaSymbol)
				{
					vint variableIndex = meta->argumentNames.Count() + meta->localVariableNames.Add(L"<recursive-lambda>" + recursiveLambdaSymbol->name);
					functionContext->localVariables.Add(recursiveLambdaSymbol.Obj(), variableIndex);
				}
					
				meta->firstInstruction = context.assembly->instructions.Count();
				if (recursiveLambdaSymbol)
				{
					INSTRUCTION(Ins::LoadClosureContext());
					INSTRUCTION(Ins::LoadFunction(context.assembly->functions.IndexOf(meta.Obj())));
					INSTRUCTION(Ins::CreateClosure());
					INSTRUCTION(Ins::StoreLocalVar(functionContext->localVariables[recursiveLambdaSymbol.Obj()]));
				}
				return functionContext;
			}

			void GenerateFunctionInstructions_Epilog(WfCodegenContext& context, WfLexicalScope* scope, Ptr<WfAssemblyFunction> meta, Ptr<ITypeInfo> returnType, Ptr<WfLexicalSymbol> recursiveLambdaSymbol, const List<Ptr<WfLexicalSymbol>>& argumentSymbols, const List<Ptr<WfLexicalSymbol>>& capturedSymbols, Ptr<WfCodegenFunctionContext> functionContext, glr::ParsingAstBase* node)
			{
				INSTRUCTION(Ins::LoadValue({}));
				INSTRUCTION(Ins::Return());
				meta->lastInstruction = context.assembly->instructions.Count() - 1;
				context.functionContext = 0;

				GenerateClosureInstructions(context, functionContext);
			}

			void GenerateFunctionInstructions(WfCodegenContext& context, WfLexicalScope* scope, Ptr<WfAssemblyFunction> meta, Ptr<ITypeInfo> returnType, Ptr<WfLexicalSymbol> recursiveLambdaSymbol, const List<Ptr<WfLexicalSymbol>>& argumentSymbols, const List<Ptr<WfLexicalSymbol>>& capturedSymbols, Ptr<WfStatement> statementBody, glr::ParsingAstBase* node)
			{
				auto functionContext = GenerateFunctionInstructions_Prolog(context, scope, meta, returnType, recursiveLambdaSymbol, argumentSymbols, capturedSymbols, node);
				GenerateStatementInstructions(context, statementBody);
				GenerateFunctionInstructions_Epilog(context, scope, meta, returnType, recursiveLambdaSymbol, argumentSymbols, capturedSymbols, functionContext, node);
			}

			void GenerateFunctionInstructions(WfCodegenContext& context, WfLexicalScope* scope, Ptr<WfAssemblyFunction> meta, Ptr<ITypeInfo> returnType, Ptr<WfLexicalSymbol> recursiveLambdaSymbol, const List<Ptr<WfLexicalSymbol>>& argumentSymbols, const List<Ptr<WfLexicalSymbol>>& capturedSymbols, Ptr<WfExpression> expressionBody, glr::ParsingAstBase* node)
			{
				auto functionContext = GenerateFunctionInstructions_Prolog(context, scope, meta, returnType, recursiveLambdaSymbol, argumentSymbols, capturedSymbols, node);
				GenerateExpressionInstructions(context, expressionBody);
				INSTRUCTION(Ins::Return());
				GenerateFunctionInstructions_Epilog(context, scope, meta, returnType, recursiveLambdaSymbol, argumentSymbols, capturedSymbols, functionContext, node);
			}

			void GenerateFunctionDeclarationInstructions(WfCodegenContext& context, WfFunctionDeclaration* node, WfLexicalScope* scope, Ptr<WfAssemblyFunction> meta, Ptr<WfLexicalSymbol> recursiveLambdaSymbol)
			{
				List<Ptr<WfLexicalSymbol>> argumentSymbols, capturedSymbols;
				{
					for (auto argument : node->arguments)
					{
						auto symbol = scope->symbols[argument->name.value][0];
						argumentSymbols.Add(symbol);
					}

					vint index = context.manager->lambdaCaptures.Keys().IndexOf(node);
					if (index != -1)
					{
						auto capture = context.manager->lambdaCaptures.Values()[index];
						for (auto symbol : capture->symbols)
						{
							capturedSymbols.Add(symbol);
						}
					}
				}

				auto returnType = CreateTypeInfoFromType(scope, node->returnType);
				GenerateFunctionInstructions(context, scope, meta, returnType, recursiveLambdaSymbol, argumentSymbols, capturedSymbols, node->statement, node);
			}

			class GenerateClassMemberInstructionsVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCodegenContext&						context;
				WfClassDeclaration*						classDecl;

				GenerateClassMemberInstructionsVisitor(WfCodegenContext& _context, WfClassDeclaration* _classDecl)
					:context(_context)
					, classDecl(_classDecl)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (node->functionKind == WfFunctionKind::Static)
					{
						GenerateDeclarationInstructions(context, node);
					}
					else if (classDecl->kind == WfClassKind::Class)
					{
						GenerateDeclarationInstructions(context, node);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				class InitializeFieldVisitor
					: public empty_visitor::DeclarationVisitor
				{
				public:
					WfCodegenContext&					context;

					InitializeFieldVisitor(WfCodegenContext& _context)
						:context(_context)
					{
					}

					void Dispatch(WfVirtualCfeDeclaration* node)override
					{
						for (auto decl : node->expandedDeclarations)
						{
							decl->Accept(this);
						}
					}

					void Dispatch(WfVirtualCseDeclaration* node)override
					{
						for (auto decl : node->expandedDeclarations)
						{
							decl->Accept(this);
						}
					}

					void Visit(WfVariableDeclaration* node)override
					{
						auto info = context.manager->declarationMemberInfos[node].Cast<WfField>().Obj();
						GenerateExpressionInstructions(context, node->expression);
						INSTRUCTION(Ins::LoadCapturedVar(0));
						INSTRUCTION(Ins::SetProperty(info));
					}
				};

				void Visit(WfConstructorDeclaration* node)override
				{
					auto meta = context.assembly->functions[context.constructors[node]];
					auto functionContext = Ptr(new WfCodegenFunctionContext);
					functionContext->function = meta;
					context.functionContext = functionContext;
					meta->firstInstruction = context.assembly->instructions.Count();
					
					auto scope = context.manager->nodeScopes[node].Obj();
					{
						for (auto [argument, index] : indexed(node->arguments))
						{
							auto symbol = scope->symbols[argument->name.value][0];
							functionContext->arguments.Add(symbol.Obj(), index);
						}
					}

					if (classDecl->baseTypes.Count() > 0)
					{
						auto td = scope->parentScope->typeOfThisExpr;
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(i);
							auto ctor = context.manager->baseConstructorCallResolvings[{node, baseTd}];
							if (ctor.key)
							{
								for (auto argument : ctor.key->arguments)
								{
									GenerateExpressionInstructions(context, argument);
								}
								INSTRUCTION(Ins::LoadCapturedVar(0));
								INSTRUCTION(Ins::InvokeBaseCtor(ctor.value, ctor.key->arguments.Count()));
							}
							else
							{
								INSTRUCTION(Ins::LoadCapturedVar(0));
								INSTRUCTION(Ins::InvokeBaseCtor(ctor.value, 0));
							}
							INSTRUCTION(Ins::Pop());
						}
					}

					{
						InitializeFieldVisitor visitor(context);
						for (auto memberDecl : classDecl->declarations)
						{
							memberDecl->Accept(&visitor);
						}
					}
					GenerateStatementInstructions(context, node->statement);

					INSTRUCTION(Ins::LoadValue({}));
					INSTRUCTION(Ins::Return());
					meta->lastInstruction = context.assembly->instructions.Count() - 1;
					context.functionContext = 0;
					GenerateClosureInstructions(context, functionContext);
				}
				
				void Visit(WfDestructorDeclaration* node)override
				{
					auto meta = context.assembly->functions[context.destructors[node]];
					auto functionContext = Ptr(new WfCodegenFunctionContext);
					functionContext->function = meta;
					context.functionContext = functionContext;
					meta->firstInstruction = context.assembly->instructions.Count();
					
					auto scope = context.manager->nodeScopes[node].Obj();
					GenerateStatementInstructions(context, node->statement);

					INSTRUCTION(Ins::LoadValue({}));
					INSTRUCTION(Ins::Return());
					meta->lastInstruction = context.assembly->instructions.Count() - 1;
					context.functionContext = 0;
					GenerateClosureInstructions(context, functionContext);
				}

				void Visit(WfClassDeclaration* node)override
				{
					GenerateDeclarationInstructions(context, node);
				}

				void Visit(WfEnumDeclaration* node)override
				{
				}

				void Visit(WfStructDeclaration* node)override
				{
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}
			};

			class GenerateDeclarationInstructionsVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCodegenContext&						context;

				GenerateDeclarationInstructionsVisitor(WfCodegenContext& _context)
					:context(_context)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					for (auto decl : node->declarations)
					{
						GenerateDeclarationInstructions(context, decl.Obj());
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					auto scope = context.manager->nodeScopes[node].Obj();
					auto symbol = context.manager->GetDeclarationSymbol(scope, node);
					auto meta = context.assembly->functions[context.globalFunctions[symbol.Obj()]];
					GenerateFunctionDeclarationInstructions(context, node, scope, meta, 0);
				}

				void Visit(WfVariableDeclaration* node)override
				{
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
				}

				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
					for (auto memberDecl : node->declarations)
					{
						GenerateClassMemberInstructionsVisitor visitor(context, node);
						memberDecl->Accept(&visitor);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
				}

				void Visit(WfStructDeclaration* node)override
				{
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}
			};

			void GenerateDeclarationInstructions(WfCodegenContext& context, WfDeclaration* declaration)
			{
				GenerateDeclarationInstructionsVisitor visitor(context);
				declaration->Accept(&visitor);
			}

/***********************************************************************
GenerateInstructions(Closure)
***********************************************************************/

			void GenerateClosureInstructions_Function(WfCodegenContext& context, vint functionIndex, WfFunctionDeclaration* node, bool createInterface)
			{
				auto scope = context.manager->nodeScopes[node].Obj();
				auto meta = context.assembly->functions[functionIndex];
				GenerateFunctionDeclarationMetadata(context, node, meta);
				Ptr<WfLexicalSymbol> recursiveLambdaSymbol;
				if (!createInterface && node->name.value != L"")
				{
					recursiveLambdaSymbol = scope->symbols[node->name.value][0];
				}
				GenerateFunctionDeclarationInstructions(context, node, scope, meta, recursiveLambdaSymbol);
			}

			void GenerateClosureInstructions_Ordered(WfCodegenContext& context, vint functionIndex, WfOrderedLambdaExpression* node)
			{
				auto scope = context.manager->nodeScopes[node].Obj();
				List<Ptr<WfLexicalSymbol>> argumentSymbols, capturedSymbols;
				CopyFrom(
					argumentSymbols,
					Range<vint>(0, scope->symbols.Count())
						.Select([scope](vint index)->Ptr<WfLexicalSymbol>{return scope->symbols.GetByIndex(index)[0];})
						.OrderBy([](Ptr<WfLexicalSymbol> a, Ptr<WfLexicalSymbol> b)
						{
							vint aId = wtoi(a->name.Sub(1, a->name.Length() - 1));
							vint bId = wtoi(b->name.Sub(1, a->name.Length() - 1));
							return aId <=> bId;
						})
					);

				auto meta = context.assembly->functions[functionIndex];
				for (auto symbol : argumentSymbols)
				{
					meta->argumentNames.Add(symbol->name);
				}
				{
					auto capture = context.manager->lambdaCaptures.Get(node);
					for (auto symbol : capture->symbols)
					{
						meta->capturedVariableNames.Add(L"<captured>" + symbol->name);
						capturedSymbols.Add(symbol);
					}

					vint count = context.GetThisStackCount(scope);
					for (vint i = 0; i < count; i++)
					{
						meta->capturedVariableNames.Add(L"<captured-this>" + itow(i));
					}
				}

				auto result = context.manager->expressionResolvings[node];
				auto returnType = CopyTypeInfo(result.type->GetElementType()->GetGenericArgument(0));
				GenerateFunctionInstructions(context, scope, meta, returnType, 0, argumentSymbols, capturedSymbols, node->body, node);
			}

			void GenerateClosureInstructions(WfCodegenContext& context, Ptr<WfCodegenFunctionContext> functionContext)
			{
				for (vint i = 0; i < functionContext->closuresToCodegen.Count(); i++)
				{
					vint functionIndex = functionContext->closuresToCodegen.Keys()[i];
					auto closure = functionContext->closuresToCodegen.Values()[i];
					
					if (closure.functionExpression)
					{
						GenerateClosureInstructions_Function(context, functionIndex, closure.functionExpression->function.Obj(), false);
					}
					else if (closure.orderedLambdaExpression)
					{
						GenerateClosureInstructions_Ordered(context, functionIndex, closure.orderedLambdaExpression);
					}
					else if (closure.functionDeclaration)
					{
						GenerateClosureInstructions_Function(context, functionIndex, closure.functionDeclaration, true);
					}
				}
			}

#undef INSTRUCTION
		}
	}
}