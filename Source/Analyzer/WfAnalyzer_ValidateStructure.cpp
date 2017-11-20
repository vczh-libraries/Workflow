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
ValidateStructureContext
***********************************************************************/

			ValidateStructureContext::ValidateStructureContext()
				:currentBindExpression(0)
				, currentObserveExpression(0)
				, currentLoopStatement(0)
				, currentCatchStatement(0)
			{
			}

/***********************************************************************
ValidateStructure(Type)
***********************************************************************/

			class ValidateReferenceTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				bool									result;

				ValidateReferenceTypeVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
					, result(true)
				{
				}
				
				void Visit(WfPredefinedType* node)override
				{
				}

				void Visit(WfTopQualifiedType* node)override
				{
				}

				void Visit(WfReferenceType* node)override
				{
				}

				void Visit(WfRawPointerType* node)override
				{
					result = false;
				}

				void Visit(WfSharedPointerType* node)override
				{
					result = false;
				}

				void Visit(WfNullableType* node)override
				{
					result = false;
				}

				void Visit(WfEnumerableType* node)override
				{
					result = false;
				}

				void Visit(WfMapType* node)override
				{
					result = false;
				}

				void Visit(WfObservableListType* node)override
				{
					result = false;
				}

				void Visit(WfFunctionType* node)override
				{
					result = false;
				}

				void Visit(WfChildType* node)override
				{
					node->parent->Accept(this);
				}

				static bool Execute(Ptr<WfType> type, WfLexicalScopeManager* manager)
				{
					ValidateReferenceTypeVisitor visitor(manager);
					type->Accept(&visitor);
					return visitor.result;
				}
			};

			class ValidateStructureTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateTypeStragety					strategy;
				WfClassDeclaration*						classDecl;

				ValidateStructureTypeVisitor(WfLexicalScopeManager* _manager, ValidateTypeStragety _strategy, WfClassDeclaration* _classDecl)
					:manager(_manager)
					, strategy(_strategy)
					, classDecl(_classDecl)
				{
				}
				
				void Visit(WfPredefinedType* node)override
				{
					switch (node->name)
					{
					case WfPredefinedTypeName::Void:
						switch (strategy)
						{
						case ValidateTypeStragety::Value:
							manager->errors.Add(WfErrors::WrongVoidType(node));
							break;
						case ValidateTypeStragety::BaseType:
							manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
						default:;
						}
						break;
					case WfPredefinedTypeName::Interface:
						switch (strategy)
						{
						case ValidateTypeStragety::ReturnType:
						case ValidateTypeStragety::Value:
							manager->errors.Add(WfErrors::WrongInterfaceType(node));
							break;
						default:;
						}
					default:;
					}
				}

				void Visit(WfTopQualifiedType* node)override
				{
				}

				void Visit(WfReferenceType* node)override
				{
				}

				void Visit(WfRawPointerType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::RawPointerToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						if (predefinedType->name != WfPredefinedTypeName::Interface)
						{
							manager->errors.Add(WfErrors::RawPointerToNonReferenceType(node));
						}
					}
				}

				void Visit(WfSharedPointerType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::SharedPointerToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						if (predefinedType->name != WfPredefinedTypeName::Interface)
						{
							manager->errors.Add(WfErrors::SharedPointerToNonReferenceType(node));
						}
					}
				}

				void Visit(WfNullableType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::NullableToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						switch (predefinedType->name)
						{
						case WfPredefinedTypeName::Void:
						case WfPredefinedTypeName::Object:
						case WfPredefinedTypeName::Interface:
							manager->errors.Add(WfErrors::NullableToNonReferenceType(node));
							break;
						default:;
						}
					}
				}

				void Visit(WfEnumerableType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->element);
				}

				void Visit(WfMapType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (node->key)
					{
						ValidateTypeStructure(manager, node->key);
					}
					ValidateTypeStructure(manager, node->value);
				}

				void Visit(WfObservableListType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->element);
				}

				void Visit(WfFunctionType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->result, ValidateTypeStragety::ReturnType);
					FOREACH(Ptr<WfType>, argument, node->arguments)
					{
						ValidateTypeStructure(manager, argument);
					}
				}

				void Visit(WfChildType* node)override
				{
					if (!ValidateReferenceTypeVisitor::Execute(node->parent, manager))
					{
						manager->errors.Add(WfErrors::ChildOfNonReferenceType(node));
					}
				}

				static void Execute(Ptr<WfType> type, WfLexicalScopeManager* manager, ValidateTypeStragety strategy, WfClassDeclaration* classDecl)
				{
					ValidateStructureTypeVisitor visitor(manager, strategy, classDecl);
					type->Accept(&visitor);
				}
			};

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
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}
					if (node->classMember)
					{
						manager->errors.Add(WfErrors::WrongClassMember(node));
					}

					for (vint i = 0; i < node->declarations.Count(); i++)
					{
						ValidateDeclarationStructure(manager, node->declarations[i]);
					}
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (classDecl && node->classMember)
					{
						switch (classDecl->kind)
						{
						case WfClassKind::Class:
							{
								switch (node->classMember->kind)
								{
								case WfClassMemberKind::Normal:
								case WfClassMemberKind::Static:
									if (!node->statement)
									{
										manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
									}
									break;
								case WfClassMemberKind::Override:
									manager->errors.Add(WfErrors::OverrideShouldImplementInterfaceMethod(node));
									break;
								}
							}
							break;
						case WfClassKind::Interface:
							{
								switch (node->classMember->kind)
								{
								case WfClassMemberKind::Normal:
									if (node->statement)
									{
										manager->errors.Add(WfErrors::InterfaceMethodShouldNotHaveImplementation(node));
									}
									break;
								case WfClassMemberKind::Static:
									if (!node->statement)
									{
										manager->errors.Add(WfErrors::FunctionShouldHaveImplementation(node));
									}
									break;
								case WfClassMemberKind::Override:
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
							switch (node->classMember->kind)
							{
							case WfClassMemberKind::Normal:
								break;
							case WfClassMemberKind::Static:
								manager->errors.Add(WfErrors::FunctionInNewTypeExpressionCannotBeStatic(node));
								break;
							case WfClassMemberKind::Override:
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
					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
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
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}
					}

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
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}

						FOREACH(Ptr<WfType>, argument, node->arguments)
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
						FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
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
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}

						ValidateTypeStructure(manager, node->type);
						FindPropertyRelatedDeclVisitor visitor(manager, classDecl, node);

						FOREACH(Ptr<WfDeclaration>, memberDecl, classDecl->declarations)
						{
							visitor.Execute(memberDecl);
						}

						if (!visitor.getter || visitor.getter->classMember->kind == WfClassMemberKind::Static || !visitor.getter.Cast<WfFunctionDeclaration>())
						{
							manager->errors.Add(WfErrors::PropertyGetterNotFound(node, classDecl));
						}

						if (node->setter.value != L"" && (!visitor.setter || visitor.setter->classMember->kind == WfClassMemberKind::Static || !visitor.setter.Cast<WfFunctionDeclaration>()))
						{
							manager->errors.Add(WfErrors::PropertySetterNotFound(node, classDecl));
						}

						if (node->valueChangedEvent.value != L"" && (!visitor.valueChangedEvent || visitor.valueChangedEvent->classMember->kind == WfClassMemberKind::Static || !visitor.valueChangedEvent.Cast<WfEventDeclaration>()))
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
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}

						if (classDecl->kind != WfClassKind::Class)
						{
							manager->errors.Add(WfErrors::WrongDeclaration(node));
						}

						FOREACH(Ptr<WfBaseConstructorCall>, call, node->baseConstructorCalls)
						{
							ValidateTypeStructure(manager, call->type, ValidateTypeStragety::BaseType, classDecl);
							FOREACH(Ptr<WfExpression>, argument, call->arguments)
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
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}

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
						FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
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
						FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
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
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
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
								FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
								{
									memberDecl->Accept(&visitor);
								}

								if (!visitor.ctor)
								{
									auto ctor = MakePtr<WfConstructorDeclaration>();
									node->declarations.Add(ctor);
									ctor->codeRange = node->codeRange;
									ctor->classMember = MakePtr<WfClassMember>();
									ctor->classMember->codeRange = node->codeRange;
									ctor->classMember->kind = WfClassMemberKind::Normal;

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

					FOREACH(Ptr<WfType>, type, node->baseTypes)
					{
						ValidateTypeStructure(manager, type, ValidateTypeStragety::BaseType, node);
					}

					{
						TooManyDtorVisitor visitor(manager, node);
						FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
						{
							ValidateDeclarationStructure(manager, memberDecl, node);
							memberDecl->Accept(&visitor);
						}
					}
				}

				void Visit(WfEnumDeclaration* node)override
				{
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					vuint64_t current = 0;
					bool reportedNotConsecutive = false;
					SortedList<WString> discoveredItems;
					FOREACH(Ptr<WfEnumItem>, item, node->items)
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
							FOREACH(Ptr<WfEnumItemIntersection>, enumInt, item->intersections)
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
					if (classDecl && node->classMember)
					{
						switch (node->classMember->kind)
						{
						case WfClassMemberKind::Normal:
							break;
						case WfClassMemberKind::Static:
						case WfClassMemberKind::Override:
							manager->errors.Add(WfErrors::WrongClassMemberConfig(node));
							break;
						}
					}
					else if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
					{
						manager->errors.Add(WfErrors::WrongDeclarationInInterfaceConstructor(node));
					}

					SortedList<WString> discoveredItems;
					FOREACH(Ptr<WfStructMember>, member, node->members)
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
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfAutoPropertyDeclaration* node)override
				{
					switch (node->classMember->kind)
					{
					case WfClassMemberKind::Normal:
						if (dynamic_cast<WfNewInterfaceExpression*>(surroundingLambda))
						{
							manager->errors.Add(WfErrors::AutoPropertyCannotBeNormalOutsideOfClass(node));
						}
						break;
					case WfClassMemberKind::Static:
						manager->errors.Add(WfErrors::AutoPropertyCannotBeStatic(node));
						break;
					case WfClassMemberKind::Override:
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
					throw 0;
				}

				static void Execute(Ptr<WfDeclaration> declaration, WfLexicalScopeManager* manager, WfClassDeclaration* classDecl, WfExpression* surroundingLambda)
				{
					ValidateStructureDeclarationVisitor visitor(manager, classDecl, surroundingLambda);
					declaration->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateStructure(Statement)
***********************************************************************/

			class ValidateStructureStatementVisitor
				: public Object
				, public WfStatement::IVisitor
				, public WfVirtualCseStatement::IVisitor
				, public WfCoroutineStatement::IVisitor
				, public WfStateMachineStatement::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateStructureContext*				context;
				Ptr<WfStatement>						result;

				ValidateStructureStatementVisitor(WfLexicalScopeManager* _manager, ValidateStructureContext* _context)
					:manager(_manager)
					, context(_context)
				{
				}

				void Visit(WfBreakStatement* node)override
				{
					if (!context->currentLoopStatement)
					{
						manager->errors.Add(WfErrors::BreakNotInLoop(node));
					}
				}

				void Visit(WfContinueStatement* node)override
				{
					if (!context->currentLoopStatement)
					{
						manager->errors.Add(WfErrors::ContinueNotInLoop(node));
					}
				}

				void Visit(WfReturnStatement* node)override
				{
					if (node->expression)
					{
						ValidateExpressionStructure(manager, context, node->expression);
					}
				}

				void Visit(WfDeleteStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					if (node->expression)
					{
						ValidateExpressionStructure(manager, context, node->expression);
					}
					else if (!context->currentCatchStatement)
					{
						manager->errors.Add(WfErrors::RethrowNotInCatch(node));
					}
				}

				void Visit(WfIfStatement* node)override
				{
					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					ValidateExpressionStructure(manager, context, node->expression);
					ValidateStatementStructure(manager, context, node->trueBranch);
					if (node->falseBranch)
					{
						ValidateStatementStructure(manager, context, node->falseBranch);
					}
				}

				void Visit(WfWhileStatement* node)override
				{
					auto oldLoop = context->currentLoopStatement;
					context->currentLoopStatement = node;
					ValidateExpressionStructure(manager, context, node->condition);
					ValidateStatementStructure(manager, context, node->statement);
					context->currentLoopStatement = oldLoop;
				}

				void Visit(WfTryStatement* node)override
				{
					ValidateStatementStructure(manager, context, node->protectedStatement);
					if (node->catchStatement)
					{
						auto oldCatch = context->currentCatchStatement;
						context->currentCatchStatement = node->catchStatement.Obj();
						ValidateStatementStructure(manager, context, node->catchStatement);
						context->currentCatchStatement = oldCatch;
					}
					if (node->finallyStatement)
					{
						ValidateStatementStructure(manager, context, node->finallyStatement);
					}
					if (!node->catchStatement && !node->finallyStatement)
					{
						manager->errors.Add(WfErrors::TryMissCatchAndFinally(node));
					}
				}

				void Visit(WfBlockStatement* node)override
				{
					for (vint i = 0; i < node->statements.Count(); i++)
					{
						ValidateStatementStructure(manager, context, node->statements[i]);
					}
				}

				void Visit(WfGotoStatement* node)override
				{
				}

				void Visit(WfExpressionStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfVariableStatement* node)override
				{
					ValidateDeclarationStructure(manager, node->variable);
				}

				void Visit(WfVirtualCseStatement* node)override
				{
					node->Accept((WfVirtualCseStatement::IVisitor*)this);
				}

				void Visit(WfSwitchStatement* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
					FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
					{
						ValidateExpressionStructure(manager, context, switchCase->expression);
						ValidateStatementStructure(manager, context, switchCase->statement);
					}
					if (node->defaultBranch)
					{
						ValidateStatementStructure(manager, context, node->defaultBranch);
					}
				}

				void Visit(WfForEachStatement* node)override
				{
					auto oldLoop = context->currentLoopStatement;
					context->currentLoopStatement = node;
					ValidateExpressionStructure(manager, context, node->collection);
					ValidateStatementStructure(manager, context, node->statement);
					context->currentLoopStatement = oldLoop;
				}

				void Visit(WfCoProviderStatement* node)override
				{
					auto oldProvider = context->currentCoProviderStatement;
					context->currentCoProviderStatement = node;
					ValidateStatementStructure(manager, context, node->statement);
					context->currentCoProviderStatement = oldProvider;
				}

				void Visit(WfCoroutineStatement* node)override
				{
					node->Accept((WfCoroutineStatement::IVisitor*)this);
				}

				void Visit(WfCoPauseStatement* node)override
				{
					if (!context->currentNewCoroutineExpression || context->currentCoPauseStatement)
					{
						manager->errors.Add(WfErrors::WrongCoPause(node));
					}
					if (node->statement)
					{
						auto oldCpPause = context->currentCoPauseStatement;
						context->currentCoPauseStatement = node;
						ValidateStatementStructure(manager, context, node->statement);
						context->currentCoPauseStatement = oldCpPause;
					}
				}

				void Visit(WfCoOperatorStatement* node)override
				{
					if (!context->currentCoProviderStatement)
					{
						manager->errors.Add(WfErrors::WrongCoOperator(node));
					}
				}

				void Visit(WfStateMachineStatement* node)override
				{
					node->Accept((WfStateMachineStatement::IVisitor*)this);
				}

				void Visit(WfStateSwitchStatement* node)override
				{
					throw 0;
				}

				void Visit(WfStateInvokeStatement* node)override
				{
					throw 0;
				}

				static void Execute(Ptr<WfStatement>& statement, WfLexicalScopeManager* manager, ValidateStructureContext* context)
				{
					ValidateStructureStatementVisitor visitor(manager, context);
					statement->Accept(&visitor);
					if (visitor.result)
					{
						statement = visitor.result;
					}
				}
			};

/***********************************************************************
ValidateStructure(Expression)
***********************************************************************/

			class ValidateStructureExpressionVisitor
				: public Object
				, public WfExpression::IVisitor
				, public WfVirtualCseExpression::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateStructureContext*				context;
				Ptr<WfExpression>						result;

				ValidateStructureExpressionVisitor(WfLexicalScopeManager* _manager, ValidateStructureContext* _context)
					:manager(_manager)
					, context(_context)
				{
				}

				void Visit(WfThisExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::WrongThisExpression(node));
					}
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
				}

				void Visit(WfReferenceExpression* node)override
				{
				}

				void Visit(WfOrderedNameExpression* node)override
				{
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					auto oldBind = context->currentBindExpression;
					context->currentBindExpression = 0;
					ValidateExpressionStructure(manager, context, node->body);
					context->currentBindExpression = oldBind;
				}

				void Visit(WfMemberExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->parent);
				}

				void Visit(WfChildExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->parent);
				}

				void Visit(WfLiteralExpression* node)override
				{
				}

				void Visit(WfFloatingExpression* node)override
				{
				}

				void Visit(WfIntegerExpression* node)override
				{
				}

				void Visit(WfStringExpression* node)override
				{
				}

				void Visit(WfUnaryExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->operand);
				}

				void Visit(WfBinaryExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->first);
					ValidateExpressionStructure(manager, context, node->second);
				}

				void Visit(WfLetExpression* node)override
				{
					FOREACH(Ptr<WfLetVariable>, variable, node->variables)
					{
						ValidateExpressionStructure(manager, context, variable->value);
					}
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfIfExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->condition);
					ValidateExpressionStructure(manager, context, node->trueBranch);
					ValidateExpressionStructure(manager, context, node->falseBranch);
				}

				void Visit(WfRangeExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->begin);
					ValidateExpressionStructure(manager, context, node->end);
				}

				void Visit(WfSetTestingExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->element);
					ValidateExpressionStructure(manager, context, node->collection);
				}

				void Visit(WfConstructorExpression* node)override
				{
					vint listElementCount = 0;
					vint mapElementCount = 0;
					FOREACH(Ptr<WfConstructorArgument>, argument, node->arguments)
					{
						ValidateExpressionStructure(manager, context, argument->key);
						if (argument->value)
						{
							ValidateExpressionStructure(manager, context, argument->value);
							mapElementCount++;
						}
						else
						{
							listElementCount++;
						}
					}

					if (listElementCount*mapElementCount != 0)
					{
						manager->errors.Add(WfErrors::ConstructorMixMapAndList(node));
					}
				}

				void Visit(WfInferExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					if (node->type)
					{
						ValidateTypeStructure(manager, node->type);
					}
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfAttachEventExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::AttachInBind(node));
					}
					ValidateExpressionStructure(manager, context, node->event);
					ValidateExpressionStructure(manager, context, node->function);
				}

				void Visit(WfDetachEventExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::DetachInBind(node));
					}
					ValidateExpressionStructure(manager, context, node->event);
					ValidateExpressionStructure(manager, context, node->handler);
				}

				void Visit(WfObserveExpression* node)override
				{
					if (!context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::ObserveNotInBind(node));
					}
					if (context->currentObserveExpression)
					{
						manager->errors.Add(WfErrors::ObserveInObserveEvent(node));
					}

					if (node->observeType == WfObserveType::SimpleObserve)
					{
						if (!node->expression.Cast<WfReferenceExpression>())
						{
							manager->errors.Add(WfErrors::WrongSimpleObserveExpression(node->expression.Obj()));
						}
						FOREACH(Ptr<WfExpression>, event, node->events)
						{
							if (!event.Cast<WfReferenceExpression>())
							{
								manager->errors.Add(WfErrors::WrongSimpleObserveEvent(event.Obj()));
							}
						}
					}
					if (node->events.Count() == 0)
					{
						manager->errors.Add(WfErrors::EmptyObserveEvent(node));
					}

					ValidateExpressionStructure(manager, context, node->parent);
					ValidateExpressionStructure(manager, context, node->expression);
					context->currentObserveExpression = node;
					for (vint i = 0; i < node->events.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->events[i]);
					}
					context->currentObserveExpression = 0;
				}

				void Visit(WfCallExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->function);
					for (vint i = 0; i < node->arguments.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->arguments[i]);
					}
				}

				void Visit(WfFunctionExpression* node)override
				{
					ValidateDeclarationStructure(manager, node->function, nullptr, node);
				}

				void Visit(WfNewClassExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					for (vint i = 0; i < node->arguments.Count(); i++)
					{
						ValidateExpressionStructure(manager, context, node->arguments[i]);
					}
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
					{
						ValidateDeclarationStructure(manager, memberDecl, nullptr, node);
					}
				}

				void Visit(WfVirtualCfeExpression* node)override
				{
					if (node->expandedExpression)
					{
						ValidateExpressionStructure(manager, context, node->expandedExpression);
					}
				}

				void Visit(WfVirtualCseExpression* node)override
				{
					node->Accept((WfVirtualCseExpression::IVisitor*)this);
				}

				void Visit(WfBindExpression* node)override
				{
					if (context->currentBindExpression)
					{
						manager->errors.Add(WfErrors::BindInBind(node));
					}

					auto bind = context->currentBindExpression;
					context->currentBindExpression = node;
					ValidateExpressionStructure(manager, context, node->expression);
					context->currentBindExpression = bind;
				}

				void Visit(WfNewCoroutineExpression* node)override
				{
					ValidateStructureContext context;
					context.currentNewCoroutineExpression = node;
					ValidateStatementStructure(manager, &context, node->statement);
				}

				void Visit(WfMixinCastExpression* node)override
				{
					ValidateTypeStructure(manager, node->type);
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfExpectedTypeCastExpression* node)override
				{
					ValidateExpressionStructure(manager, context, node->expression);
				}

				void Visit(WfCoOperatorExpression* node)override
				{
					if (!context->currentCoProviderStatement)
					{
						manager->errors.Add(WfErrors::WrongCoOperator(node));
					}
				}

				static void Execute(Ptr<WfExpression>& expression, WfLexicalScopeManager* manager, ValidateStructureContext* context)
				{
					ValidateStructureExpressionVisitor visitor(manager, context);
					expression->Accept(&visitor);
					if (visitor.result)
					{
						expression = visitor.result;
					}
				}
			};

/***********************************************************************
ValidateStructure
***********************************************************************/

			void ValidateTypeStructure(WfLexicalScopeManager* manager, Ptr<WfType> type, ValidateTypeStragety strategy, WfClassDeclaration* classDecl)
			{
				ValidateStructureTypeVisitor::Execute(type, manager, strategy, classDecl);
			}

			void ValidateModuleStructure(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				FOREACH(Ptr<WfModuleUsingPath>, path, module->paths)
				{
					FOREACH_INDEXER(Ptr<WfModuleUsingItem>, item, index, path->items)
					{
						vint counter = 0;
						FOREACH(Ptr<WfModuleUsingFragment>, fragment, item->fragments)
						{
							if (fragment.Cast<WfModuleUsingWildCardFragment>())
							{
								if (index == path->items.Count() - 1)
								{
									counter++;
								}
								else
								{
									goto USING_PATH_INCORRECT;
								}
							}
						}

						if (index == path->items.Count() - 1 && counter != 1)
						{
							goto USING_PATH_INCORRECT;
						}
					}
					continue;
				USING_PATH_INCORRECT:
					manager->errors.Add(WfErrors::WrongUsingPathWildCard(path.Obj()));
				}
				for (vint i = 0; i < module->declarations.Count(); i++)
				{
					ValidateDeclarationStructure(manager, module->declarations[i]);
				}
			}

			void ValidateDeclarationStructure(WfLexicalScopeManager* manager, Ptr<WfDeclaration> declaration, WfClassDeclaration* classDecl, WfExpression* surroundingLambda)
			{
				ValidateStructureDeclarationVisitor::Execute(declaration, manager, classDecl, surroundingLambda);
			}

			void ValidateStatementStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfStatement>& statement)
			{
				ValidateStructureStatementVisitor::Execute(statement, manager, context);
			}

			void ValidateExpressionStructure(WfLexicalScopeManager* manager, ValidateStructureContext* context, Ptr<WfExpression>& expression)
			{
				ValidateStructureExpressionVisitor::Execute(expression, manager, context);
			}
		}
	}
}
