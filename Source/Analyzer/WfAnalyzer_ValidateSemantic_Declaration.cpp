#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace typeimpl;

/***********************************************************************
ValidateSemantic(ClassMember)
***********************************************************************/

			class ValidateSemanticClassMemberVisitor
				: public Object
				, public WfDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;
				Ptr<WfCustomType>					td;
				Ptr<WfClassDeclaration>				classDecl;

				ValidateSemanticClassMemberVisitor(Ptr<WfCustomType> _td, Ptr<WfClassDeclaration> _classDecl, WfLexicalScopeManager* _manager)
					:td(_td)
					, classDecl(_classDecl)
					, manager(_manager)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfVariableDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfEventDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfPropertyDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);

					auto scope = manager->nodeScopes[node];
					if (auto typeInfo = CreateTypeInfoFromType(scope.Obj(), node->type))
					{
						if (node->getter.value != L"")
						{
							auto getter = td->GetMethodGroupByName(node->getter.value, false)->GetMethod(0);
							if (!IsSameType(typeInfo.Obj(), getter->GetReturn()) || getter->GetParameterCount() != 0)
							{
								manager->errors.Add(WfErrors::PropertyGetterTypeMismatched(node, classDecl.Obj()));
							}
						}

						if (node->setter.value != L"")
						{
							auto setter = td->GetMethodGroupByName(node->setter.value, false)->GetMethod(0);
							if (setter->GetReturn()->GetTypeDescriptor() != description::GetTypeDescriptor<void>() || setter->GetParameterCount() != 1 || !IsSameType(typeInfo.Obj(), setter->GetParameter(0)->GetType()))
							{
								manager->errors.Add(WfErrors::PropertySetterTypeMismatched(node, classDecl.Obj()));
							}
						}
					}
				}

				static void SelectConstructor(WfLexicalScopeManager* manager, WfConstructorDeclaration* ctorDecl, glr::ParsingAstBase* node, ITypeDescriptor* td, List<Ptr<WfExpression>>& arguments)
				{
					List<ResolveExpressionResult> functions;
					if (auto ctors = td->GetConstructorGroup())
					{
						vint count = ctors->GetMethodCount();
						for (vint i = 0; i < count; i++)
						{
							auto ctor = ctors->GetMethod(i);
							functions.Add(ResolveExpressionResult::Method(ctor));
						}
					}

					vint selectedFunctionIndex = -1;
					SelectFunction(manager, node, nullptr, functions, arguments, selectedFunctionIndex);
					if (selectedFunctionIndex != -1)
					{
						auto ctor = functions[selectedFunctionIndex].methodInfo;
						auto call = dynamic_cast<WfBaseConstructorCall*>(node);
						manager->baseConstructorCallResolvings.Add({ctorDecl, td}, {call, ctor});
					}
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);

					auto scope = manager->nodeScopes[node].Obj();
					auto classScope = scope->parentScope.Obj();
					SortedList<ITypeDescriptor*> baseTypes, initTypes;
					{
						auto td = classScope->typeOfThisExpr;
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(i);
							if (!baseTypes.Contains(baseTd))
							{
								baseTypes.Add(baseTd);
							}
						}
					}

					for (auto call : node->baseConstructorCalls)
					{
						if (auto scopeName = GetScopeNameFromReferenceType(classScope, call->type))
						{
							if (auto td = scopeName->typeDescriptor)
							{
								if (initTypes.Contains(td))
								{
									manager->errors.Add(WfErrors::DuplicatedBaseConstructorCall(call.Obj(), td));
								}
								else if (baseTypes.Contains(td))
								{
									initTypes.Add(td);
									SelectConstructor(manager, node, call.Obj(), td, call->arguments);
								}
								else
								{
									manager->errors.Add(WfErrors::WrongBaseConstructorCall(call.Obj(), td));
								}
							}
						}
					}

					{
						vint index = -1;
						for (vint i = 0; i < initTypes.Count(); i++)
						{
							while (baseTypes[++index] != initTypes[i])
							{
								auto td = baseTypes[index];
								List<Ptr<WfExpression>> arguments;
								SelectConstructor(manager, node, node, td, arguments);
							}
						}
						while (++index < baseTypes.Count())
						{
							auto td = baseTypes[index];
							List<Ptr<WfExpression>> arguments;
							SelectConstructor(manager, node, node, td, arguments);
						}
					}

					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
					ValidateStatementSemantic(manager, node->statement);
				}

				void Visit(WfClassDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfEnumDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfStructDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					ValidateDeclarationSemantic(manager, node);
				}

				static void Execute(Ptr<WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl, WfLexicalScopeManager* manager)
				{
					ValidateSemanticClassMemberVisitor visitor(td, classDecl, manager);
					memberDecl->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateSemantic(Declaration)
***********************************************************************/

			class ExpandVirtualDeclarationVisitor : public Object, public WfVirtualCseDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ExpandVirtualDeclarationVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Visit(WfStateMachineDeclaration* node)override
				{
					ExpandStateMachine(manager, node);
				}
			};

			class ValidateSemanticDeclarationVisitor
				: public Object
				, public WfDeclaration::IVisitor
				, public WfVirtualCseDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*				manager;

				ValidateSemanticDeclarationVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Visit(List<Ptr<WfAttribute>>& attributes)
				{
					for (auto attribute : attributes)
					{
						auto key = Pair<WString, WString>(attribute->category.value, attribute->name.value);
						vint index = manager->attributes.Keys().IndexOf(key);
						if (index == -1)
						{
							manager->errors.Add(WfErrors::AttributeNotExists(attribute.Obj()));
						}
						else
						{
							auto expectedType = manager->attributes.Values()[index];
							if (attribute->value)
							{
								ValidateConstantExpression(manager, attribute->value, expectedType);
							}
							else if (expectedType->GetTypeDescriptor() != description::GetTypeDescriptor<void>())
							{
								manager->errors.Add(WfErrors::AttributeMissValue(attribute.Obj()));
							}
						}
					}
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					for (auto declaration : node->declarations)
					{
						ValidateDeclarationSemantic(manager, declaration);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (node->statement)
					{
						ValidateStatementSemantic(manager, node->statement);
					}
					for (auto argument : node->arguments)
					{
						Visit(argument->attributes);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto symbol = scope->symbols[node->name.value][0];
					symbol->typeInfo = GetExpressionType(manager, node->expression, symbol->typeInfo);
					if (symbol->typeInfo && !symbol->type)
					{
						symbol->type = GetTypeFromTypeInfo(symbol->typeInfo.Obj());
					}
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
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Cast<WfCustomType>();

					if (node->kind == WfClassKind::Interface)
					{
						for (auto baseType : node->baseTypes)
						{
							auto scopeName = GetScopeNameFromReferenceType(scope->parentScope.Obj(), baseType);
							auto baseTd = scopeName->typeDescriptor;
							auto ctor = FindInterfaceConstructor(baseTd);
							if (ctor == nullptr)
							{
								manager->errors.Add(WfErrors::WrongInterfaceBaseType(node, baseTd));
							}
						}
					}

					if (auto sm = From(node->declarations)
						.FindType<WfStateMachineDeclaration>()
						.First(nullptr)
						)
					{
						auto smtd = description::GetTypeDescriptor<StateMachine>();
						vint count = td->GetBaseTypeDescriptorCount();

						auto smbc = Range<vint>(0, count)
							.Select([=](vint index) {return td->GetBaseTypeDescriptor(index); })
							.Where([=](ITypeDescriptor* td) {return td == smtd; })
							.First(nullptr);
						if (!smbc)
						{
							manager->errors.Add(WfErrors::StateMachineClassNotInheritFromStateMachine(node));
						}
					}

					for (auto memberDecl : node->declarations)
					{
						ValidateClassMemberSemantic(manager, td, node, memberDecl);
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					for (auto item : node->items)
					{
						Visit(item->attributes);
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Cast<WfStruct>();
					for (auto member : node->members)
					{
						auto memberTd = td->GetPropertyByName(member->name.value, false)->GetReturn()->GetTypeDescriptor();
						if ((memberTd->GetTypeDescriptorFlags() & TypeDescriptorFlags::ReferenceType) != TypeDescriptorFlags::Undefined)
						{
							manager->errors.Add(WfErrors::StructContainsNonValueType(member.Obj(), node));
						}
					}

					for (auto member : node->members)
					{
						Visit(member->attributes);
					}
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						ValidateDeclarationSemantic(manager, decl);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					bool expanded = node->expandedDeclarations.Count() > 0;
					vint errorCount = manager->errors.Count();
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);

					if (!expanded && manager->errors.Count() == errorCount)
					{
						ExpandVirtualDeclarationVisitor visitor(manager);
						node->Accept(&visitor);

						for (auto decl : node->expandedDeclarations)
						{
							SetCodeRange(decl, node->codeRange);
						}

						auto parentScope = manager->nodeScopes[node];
						if (parentScope->ownerNode == node)
						{
							parentScope = parentScope->parentScope;
						}

						for (auto decl : node->expandedDeclarations)
						{
							ContextFreeDeclarationDesugar(manager, decl);
						}
						for (auto decl : node->expandedDeclarations)
						{
							BuildScopeForDeclaration(manager, parentScope, decl, manager->declaractionScopeSources[node]);
						}

						manager->checkedScopes_DuplicatedSymbol.Remove(parentScope.Obj());
						manager->checkedScopes_SymbolType.Remove(parentScope.Obj());
						if (!CheckScopes_DuplicatedSymbol(manager) || !CheckScopes_SymbolType(manager))
						{
							return;
						}
					}

					for (auto decl : node->expandedDeclarations)
					{
						ValidateDeclarationSemantic(manager, decl);
					}
				}

				void Visit(WfStateMachineDeclaration* node)override
				{
					bool foundDefaultState = false;

					for (auto state : node->states)
					{
						if (state->name.value == L"")
						{
							foundDefaultState = true;
						}
						ValidateStatementSemantic(manager, state->statement);
					}

					if (!foundDefaultState)
					{
						manager->errors.Add(WfErrors::MissingDefaultState(node));
					}
				}

				static void Execute(Ptr<WfDeclaration> declaration, WfLexicalScopeManager* manager)
				{
					ValidateSemanticDeclarationVisitor visitor(manager);
					declaration->Accept(&visitor);
					visitor.Visit(declaration->attributes);
				}
			};

/***********************************************************************
ValidateSemantic
***********************************************************************/

			void ValidateClassMemberSemantic(WfLexicalScopeManager* manager, Ptr<typeimpl::WfCustomType> td, Ptr<WfClassDeclaration> classDecl, Ptr<WfDeclaration> memberDecl)
			{
				return ValidateSemanticClassMemberVisitor::Execute(td, classDecl, memberDecl, manager);
			}

			void ValidateDeclarationSemantic(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration)
			{
				return ValidateSemanticDeclarationVisitor::Execute(declaration, manager);
			}
		}
	}
}
