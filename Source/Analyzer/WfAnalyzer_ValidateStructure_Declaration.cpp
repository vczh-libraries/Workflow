#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection::description;

/***********************************************************************
ValidateStructure(Declaration)
***********************************************************************/

			class ValidateStructureDeclarationVisitor
				: public Object
				, public WfDeclaration::IVisitor
				, public WfVirtualCfeDeclaration::IVisitor
				, public WfVirtualCseDeclaration::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				WfClassDeclaration*						classDecl;
				WfExpression*							surroundingLambda;

				ValidateStructureDeclarationVisitor(WfLexicalScopeManager* _manager, WfClassDeclaration* _classDecl, WfExpression* _surroundingLambda)
					:manager(_manager)
					, classDecl(_classDecl)
					, surroundingLambda(_surroundingLambda)
				{
					CHECK_ERROR(!(_classDecl && _surroundingLambda), L"vl::workflow::analyzer::ValidateStructureDeclarationVisitor(WfLexicalScopeManager*, WfClassDeclaration*, WfExpression*)#Both classDecl or surroundingLambda should not exist at the same time.");
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					if (classDecl)
					{
						manager->errors.Add(WfErrors::WrongClassMember(node));
					}

					if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					for (vint i = 0; i < node->declarations.Count(); i++)
					{
						ValidateDeclarationStructure(manager, node->declarations[i]);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (classDecl)
					{
						switch (classDecl->kind)
						{
						case WfClassKind::Class:
							{
								switch (node->functionKind)
								{
								case WfFunctionKind::Normal:
								case WfFunctionKind::Static:
									if (!node->statement)
									{
										manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
									}
									break;
								case WfFunctionKind::Override:
									manager->errors.Add(WfErrors::OverrideShouldImplementInterfaceMethod(node));
									break;
								}
							}
							break;
						case WfClassKind::Interface:
							{
								switch (node->functionKind)
								{
								case WfFunctionKind::Normal:
									if (node->statement)
									{
										manager->errors.Add(WfErrors::InterfaceMethodShouldNotHaveImplementation(node));
									}
									break;
								case WfFunctionKind::Static:
									if (!node->statement)
									{
										manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
									}
									break;
								case WfFunctionKind::Override:
									manager->errors.Add(WfErrors::OverrideShouldImplementInterfaceMethod(node));
									break;
								}
							}
							break;
						}
					}
					else
					{
						if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
						{
							switch (node->functionKind)
							{
							case WfFunctionKind::Normal:
								break;
							case WfFunctionKind::Static:
								manager->errors.Add(WfErrors::FunctionInNewTypeExpressionCannotBeStatic(node));
								break;
							case WfFunctionKind::Override:
								break;
							}
						}
						if (!node->statement)
						{
							manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
						}
					}

					if (node->anonymity == WfFunctionAnonymity::Anonymous)
					{
						if (!dynamic_cast<WfFunctionExpression*>(surroundingLambda))
						{
							manager->errors.Add(WfErrors::FunctionShouldHaveName(node));
						}
					}


					ValidateTypeStructure(manager, node->returnType, ValidateTypeStragety::ReturnType);
					for (auto argument : node->arguments)
					{
						ValidateTypeStructure(manager, argument->type);
					}

					if (node->statement)
					{
						ValidateStructureContext context;
						ValidateStatementStructure(manager, &context, node->statement);
					}
				}

				void Visit(WfVariableDeclaration* node)override
				{
					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					else if (classDecl)
					{
						manager->errors.Add(WfErrors::MissingFieldType(node));
					}
					ValidateStructureContext context;
					ValidateExpressionStructure(manager, &context, node->expression);
				}

				void Visit(WfEventDeclaration* node)override
				{
					if (classDecl)
					{
						for (auto argument : node->arguments)
						{
							ValidateTypeStructure(manager, argument);
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				class FindPropertyRelatedDeclVisitor
					: public empty_visitor::DeclarationVisitor
					, public empty_visitor::VirtualCseDeclarationVisitor
				{
				public:
					WfLexicalScopeManager*				manager;
					WfClassDeclaration*					classDecl;
					WfPropertyDeclaration*				propDecl;
					Ptr<WfDeclaration>					getter;
					Ptr<WfDeclaration>					setter;
					Ptr<WfDeclaration>					valueChangedEvent;
					bool								duplicateGetter = false;
					bool								duplicateSetter = false;
					bool								duplicateEvent = false;

					FindPropertyRelatedDeclVisitor(WfLexicalScopeManager* _manager, WfClassDeclaration* _classDecl, WfPropertyDeclaration* _propDecl)
						:manager(_manager)
						, classDecl(_classDecl)
						, propDecl(_propDecl)
					{
					}

					void Dispatch(WfVirtualCfeDeclaration* node)override
					{
						for (auto decl : node->expandedDeclarations)
						{
							Execute(decl);
						}
					}

					void Dispatch(WfVirtualCseDeclaration* node)override
					{
						node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
					}

					void Execute(Ptr<WfDeclaration> memberDecl)
					{
						memberDecl->Accept(this);
						if (!duplicateGetter && memberDecl->name.value == propDecl->getter.value)
						{
							if (getter)
							{
								duplicateGetter = true;
								manager->errors.Add(WfErrors::TooManyPropertyGetter(propDecl, classDecl));
							}
							else
							{
								getter = memberDecl;
							}
						}

						if (!duplicateSetter && propDecl->setter.value != L"" && memberDecl->name.value == propDecl->setter.value)
						{
							if (setter)
							{
								duplicateSetter = true;
								manager->errors.Add(WfErrors::TooManyPropertySetter(propDecl, classDecl));
							}
							else
							{
								setter = memberDecl;
							}
						}

						if (!duplicateEvent && propDecl->valueChangedEvent.value != L"" && memberDecl->name.value == propDecl->valueChangedEvent.value)
						{
							if (valueChangedEvent)
							{
								duplicateEvent = true;
								manager->errors.Add(WfErrors::TooManyPropertyEvent(propDecl, classDecl));
							}
							else
							{
								valueChangedEvent = memberDecl;
							}
						}
					}
				};

				void Visit(WfPropertyDeclaration* node)override
				{
					if (classDecl)
					{
						ValidateTypeStructure(manager, node->type);
						FindPropertyRelatedDeclVisitor visitor(manager, classDecl, node);

						for (auto memberDecl : classDecl->declarations)
						{
							visitor.Execute(memberDecl);
						}

						if (auto getter = visitor.getter.Cast<WfFunctionDeclaration>())
						{
							if (getter->functionKind == WfFunctionKind::Static)
							{
								manager->errors.Add(WfErrors::PropertyGetterNotFound(node, classDecl));
							}
						}
						else
						{
							manager->errors.Add(WfErrors::PropertyGetterNotFound(node, classDecl));
						}

						if (node->setter.value != L"")
						{
							if (auto setter = visitor.setter.Cast<WfFunctionDeclaration>())
							{
								if (setter->functionKind == WfFunctionKind::Static)
								{
									manager->errors.Add(WfErrors::PropertyGetterNotFound(node, classDecl));
								}
							}
							else
							{
								manager->errors.Add(WfErrors::PropertyGetterNotFound(node, classDecl));
							}
						}

						if (node->valueChangedEvent.value != L"" && (!visitor.valueChangedEvent || !visitor.valueChangedEvent.Cast<WfEventDeclaration>()))
						{
							manager->errors.Add(WfErrors::PropertyEventNotFound(node, classDecl));
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					if (classDecl)
					{
						if (classDecl->kind != WfClassKind::Class)
						{
							manager->errors.Add(WfErrors::WrongDeclaration(node));
						}

						for (auto call : node->baseConstructorCalls)
						{
							ValidateTypeStructure(manager, call->type, ValidateTypeStragety::BaseType, classDecl);
							for (auto argument : call->arguments)
							{
								ValidateStructureContext context;
								ValidateExpressionStructure(manager, &context, argument);
							}
						}
						{
							ValidateStructureContext context;
							ValidateStatementStructure(manager, &context, node->statement);
						}
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					if (classDecl)
					{
						if (classDecl->kind != WfClassKind::Class)
						{
							manager->errors.Add(WfErrors::WrongDeclaration(node));
						}

						ValidateStructureContext context;
						ValidateStatementStructure(manager, &context, node->statement);
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				class FindCtorVisitor
					: public empty_visitor::DeclarationVisitor
					, public empty_visitor::VirtualCseDeclarationVisitor
				{
				public:
					WfConstructorDeclaration*			ctor = nullptr;

					void Dispatch(WfVirtualCfeDeclaration* node)override
					{
						for (auto decl : node->expandedDeclarations)
						{
							decl->Accept(this);
						}
					}

					void Dispatch(WfVirtualCseDeclaration* node)override
					{
						node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
					}

					void Visit(WfConstructorDeclaration* node)override
					{
						ctor = node;
					}
				};

				class TooManyDtorVisitor
					: public empty_visitor::DeclarationVisitor
					, public empty_visitor::VirtualCseDeclarationVisitor
				{
				public:
					WfLexicalScopeManager*				manager;
					WfClassDeclaration*					classDecl;
					WfDestructorDeclaration*			dtor = nullptr;

					TooManyDtorVisitor(WfLexicalScopeManager* _manager, WfClassDeclaration* _classDecl)
						:manager(_manager)
						, classDecl(_classDecl)
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
						node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
					}

					void Visit(WfDestructorDeclaration* node)override
					{
						if (!dtor)
						{
							dtor = node;
						}
						else
						{
							manager->errors.Add(WfErrors::TooManyDestructor(node, classDecl));
						}
					}
				};

				void Visit(WfClassDeclaration* node)override
				{
					if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					switch (node->kind)
					{
					case WfClassKind::Class:
						{
							if (node->constructorType != WfConstructorType::Undefined)
							{
								manager->errors.Add(WfErrors::ClassWithInterfaceConstructor(node));
							}
							{
								FindCtorVisitor visitor;
								for (auto memberDecl : node->declarations)
								{
									memberDecl->Accept(&visitor);
								}

								if (!visitor.ctor)
								{
									auto ctor = MakePtr<WfConstructorDeclaration>();
									node->declarations.Add(ctor);
									ctor->codeRange = node->codeRange;
									ctor->constructorType = WfConstructorType::SharedPtr;

									auto stat = MakePtr<WfBlockStatement>();
									ctor->statement = stat;
									stat->codeRange = node->codeRange;
								}
							}
						}
						break;
					case WfClassKind::Interface:
						break;
					}

					for (auto type : node->baseTypes)
					{
						ValidateTypeStructure(manager, type, ValidateTypeStragety::BaseType, node);
					}

					{
						TooManyDtorVisitor visitor(manager, node);
						bool hasStateMachine = false;
						for (auto memberDecl : node->declarations)
						{
							if (auto smDecl = memberDecl.Cast<WfStateMachineDeclaration>())
							{
								if (!hasStateMachine)
								{
									hasStateMachine = true;
								}
								else
								{
									manager->errors.Add(WfErrors::DuplicatedDeclaration(smDecl.Obj()));
								}
							}
							ValidateDeclarationStructure(manager, memberDecl, node);
							memberDecl->Accept(&visitor);
						}
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					vuint64_t current = 0;
					bool reportedNotConsecutive = false;
					SortedList<WString> discoveredItems;
					for (auto item : node->items)
					{
						switch (item->kind)
						{
						case WfEnumItemKind::Constant:
							{
								vuint64_t value = -1;
								TypedValueSerializerProvider<vuint64_t>::Deserialize(item->number.value, value);
								if (!reportedNotConsecutive && value != current)
								{
									reportedNotConsecutive = true;
									switch (node->kind)
									{
									case WfEnumKind::Normal:
										manager->errors.Add(WfErrors::EnumValuesNotConsecutiveFromZero(node));
										break;
									case WfEnumKind::Flag:
										manager->errors.Add(WfErrors::FlagValuesNotConsecutiveFromZero(node));
										break;
									}
								}
								switch (node->kind)
								{
								case WfEnumKind::Normal:
									current++;
									break;
								case WfEnumKind::Flag:
									current = current == 0 ? 1 : current * 2;
									break;
								}
							}
							break;
						case WfEnumItemKind::Intersection:
							for (auto enumInt : item->intersections)
							{
								if (!discoveredItems.Contains(enumInt->name.value))
								{
									manager->errors.Add(WfErrors::FlagValueNotExists(enumInt.Obj(), node));
								}
							}
							break;
						}

						if (discoveredItems.Contains(item->name.value))
						{
							manager->errors.Add(WfErrors::DuplicatedEnumValue(item.Obj(), node));
						}
						else
						{
							discoveredItems.Add(item->name.value);
						}
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					SortedList<WString> discoveredItems;
					for (auto member : node->members)
					{
						if (discoveredItems.Contains(member->name.value))
						{
							manager->errors.Add(WfErrors::DuplicatedStructMember(member.Obj(), node));
						}
						else
						{
							discoveredItems.Add(member->name.value);
						}
					}
				}

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					node->Accept(static_cast<WfVirtualCfeDeclaration::IVisitor*>(this));
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfAutoPropertyDeclaration* node)override
				{
					switch (node->functionKind)
					{
					case WfFunctionKind::Normal:
						if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
						{
							manager->errors.Add(WfErrors::AutoPropertyCannotBeNormalOutsideOfClass(node));
						}
						break;
					case WfFunctionKind::Static:
						manager->errors.Add(WfErrors::AutoPropertyCannotBeStatic(node));
						break;
					case WfFunctionKind::Override:
						if (!dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
						{
							manager->errors.Add(WfErrors::OverrideShouldImplementInterfaceMethod(node));
						}
						break;
					}

					if (classDecl)
					{
						switch (classDecl->kind)
						{
						case WfClassKind::Class:
							if (!node->expression)
							{
								manager->errors.Add(WfErrors::AutoPropertyShouldBeInitialized(node));
							}
							break;
						case WfClassKind::Interface:
							if (node->expression)
							{
								manager->errors.Add(WfErrors::AutoPropertyCannotBeInitializedInInterface(node, classDecl));
							}
							break;
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						if (!node->expression)
						{
							manager->errors.Add(WfErrors::AutoPropertyShouldBeInitialized(node));
						}
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}

					ValidateTypeStructure(manager, node->type);
					if (node->expression)
					{
						ValidateStructureContext context;
						ValidateExpressionStructure(manager, &context, node->expression);
					}
				}

				void Visit(WfCastResultInterfaceDeclaration* node)override
				{
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					node->Accept(static_cast<WfVirtualCseDeclaration::IVisitor*>(this));
				}

				void Visit(WfStateMachineDeclaration* node)override
				{
					if (classDecl)
					{
						for (auto state : node->states)
						{
							ValidateStructureContext context;
							context.currentStateDeclaration = state.Obj();
							ValidateStatementStructure(manager, &context, state->statement);
						}
					}
					else
					{
						manager->errors.Add(WfErrors::WrongDeclaration(node));
					}
				}

				static void Execute(Ptr<WfDeclaration> declaration, WfLexicalScopeManager* manager, WfClassDeclaration* classDecl, WfExpression* surroundingLambda)
				{
					ValidateStructureDeclarationVisitor visitor(manager, classDecl, surroundingLambda);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateStructure
***********************************************************************/

			void ValidateDeclarationStructure(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration, WfClassDeclaration* classDecl, WfExpression* surroundingLambda)
			{
				ValidateStructureDeclarationVisitor::Execute(declaration, manager, classDecl, surroundingLambda);
			}
		}
	}
}
