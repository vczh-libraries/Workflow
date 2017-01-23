#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection;
			using namespace reflection::description;
			using namespace runtime;

/***********************************************************************
WfLexicalSymbol
***********************************************************************/

			WfLexicalSymbol::WfLexicalSymbol(WfLexicalScope* _ownerScope)
				:ownerScope(_ownerScope)
			{
			}

			WfLexicalSymbol::~WfLexicalSymbol()
			{
			}

			WString WfLexicalSymbol::GetFriendlyName()
			{
				return ownerScope->GetFriendlyName() + L"::" + name;
			}

/***********************************************************************
WfLexicalScope
***********************************************************************/

			WfLexicalScope::WfLexicalScope(WfLexicalScopeManager* _ownerManager)
				:ownerManager(_ownerManager)
			{
			}

			WfLexicalScope::WfLexicalScope(Ptr<WfLexicalScope> _parentScope)
				:ownerManager(0)
				, parentScope(_parentScope)
			{
			}

			WfLexicalScope::~WfLexicalScope()
			{
			}

			WfLexicalScopeManager* WfLexicalScope::FindManager()
			{
				WfLexicalScope* scope = this;
				while (scope)
				{
					if (scope->ownerManager)
					{
						return scope->ownerManager;
					}
					scope = scope->parentScope.Obj();
				}
				return 0;
			}

			Ptr<WfModule> WfLexicalScope::FindModule()
			{
				WfLexicalScope* scope = this;
				while (scope)
				{
					if (auto module = scope->ownerNode.Cast<WfModule>())
					{
						return module;
					}
					scope = scope->parentScope.Obj();
				}
				return 0;
			}

			Ptr<WfDeclaration> WfLexicalScope::FindDeclaration()
			{
				WfLexicalScope* scope = this;
				while (scope)
				{
					if (auto decl = scope->ownerNode.Cast<WfDeclaration>())
					{
						return decl;
					}
					scope = scope->parentScope.Obj();
				}
				return 0;
			}

			WString WfLexicalScope::GetFriendlyName()
			{
				if (auto module = ownerNode.Cast<WfModule>())
				{
					return L"<" + module->name.value + L">";
				}
				else if (auto decl = ownerNode.Cast<WfDeclaration>())
				{
					auto name = decl->name.value;
					if (name == L"")
					{
						name = L"<anonymous>";
					}
					return parentScope->GetFriendlyName() + L"::" + name;
				}
				else if (!parentScope)
				{
					return L"";
				}
				else
				{
					return parentScope->GetFriendlyName() + L"::<>";
				}
			}

/***********************************************************************
WfLexicalScopeName
***********************************************************************/

			WfLexicalScopeName::WfLexicalScopeName(bool _imported)
				:imported(_imported)
			{
			}

			WfLexicalScopeName::~WfLexicalScopeName()
			{
			}

			Ptr<WfLexicalScopeName> WfLexicalScopeName::AccessChild(const WString& name, bool imported)
			{
				vint index = children.Keys().IndexOf(name);
				if (index == -1)
				{
					Ptr<WfLexicalScopeName> newName = new WfLexicalScopeName(imported);
					newName->name = name;
					newName->parent = this;
					children.Add(name, newName);
					return newName;
				}
				else
				{
					return children.Values()[index];
				}
			}

			void WfLexicalScopeName::RemoveNonTypeDescriptorNames(WfLexicalScopeManager* manager)
			{
				FOREACH(Ptr<WfLexicalScopeName>, name, children.Values())
				{
					name->RemoveNonTypeDescriptorNames(manager);
				}

				for (vint i = children.Count() - 1; i >= 0; i--)
				{
					auto subScopeName = children.Values()[i];
					if (!subScopeName->imported)
					{
						children.Remove(children.Keys()[i]);
						if (subScopeName->typeDescriptor)
						{
							manager->typeNames.Remove(subScopeName->typeDescriptor);
						}
					}
				}
				
				declarations.Clear();
			}

			WString WfLexicalScopeName::GetFriendlyName()
			{
				if (parent)
				{
					WString parentName = parent->GetFriendlyName();
					if (parentName == L"")
					{
						return name;
					}
					else
					{
						return parentName + L"::" + name;
					}
				}
				else
				{
					return L"";
				}
			}

/***********************************************************************
ResolveExpressionResult
***********************************************************************/

			WString ResolveExpressionResult::GetFriendlyName(bool upperCase)const
			{
				WString typeName, result;
				if (type)
				{
					typeName= + L" of type \"" + type->GetTypeFriendlyName() + L"\"";
				}

				if (scopeName)
				{
					result = scopeName->GetFriendlyName();
				}
				else if (symbol)
				{
					result = symbol->GetFriendlyName() + typeName;
				}
				else if (propertyInfo)
				{
					result = L"property \"" + propertyInfo->GetName() + L"\" in \"" + propertyInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"" + typeName;
				}
				else if (methodInfo)
				{
					if (methodInfo->GetName() == L"")
					{
						result = L"constructor in \"" + methodInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"" + typeName;
					}
					else
					{
						result = L"method \"" + methodInfo->GetName() + L"\" in \"" + methodInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"" + typeName;
					}
				}
				else if (eventInfo)
				{
					result = L"event \"" + eventInfo->GetName() + L"\" in \"" + eventInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"";
				}
				else if (type)
				{
					result = L"expression" + typeName;
				}
				else
				{
					result = L"<unknown>";
				}

				if (result.Length() > 0 && upperCase)
				{
					result = INVLOC.ToUpper(result.Left(1)) + result.Right(result.Length() - 1);
				}
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::ScopeName(Ptr<WfLexicalScopeName> _scopeName)
			{
				ResolveExpressionResult result;
				result.scopeName = _scopeName;
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::ReadonlySymbol(Ptr<WfLexicalSymbol> _symbol)
			{
				ResolveExpressionResult result;
				result.symbol = _symbol;
				result.type = _symbol->typeInfo;
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::Symbol(Ptr<WfLexicalSymbol> _symbol)
			{
				ResolveExpressionResult result;
				result.symbol = _symbol;
				result.type = _symbol->typeInfo;
				if (_symbol->creatorNode.Cast<WfVariableDeclaration>())
				{
					result.writableType = _symbol->typeInfo;
				}
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::ReadonlyType(Ptr<reflection::description::ITypeInfo> _type)
			{
				ResolveExpressionResult result;
				result.type = _type;
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::WritableType(Ptr<reflection::description::ITypeInfo> _type)
			{
				ResolveExpressionResult result;
				result.type = _type;
				result.writableType = _type;
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::Property(reflection::description::IPropertyInfo* _propertyInfo)
			{
				Ptr<ITypeInfo> getterType = CopyTypeInfo(_propertyInfo->GetReturn());
				Ptr<ITypeInfo> setterType;
				if (IMethodInfo* setter = _propertyInfo->GetSetter())
				{
					setterType = getterType;
					if (setter->GetParameterCount() == 1 && !IsSameType(getterType.Obj(), setter->GetParameter(0)->GetType()))
					{
						setterType = CopyTypeInfo(setter->GetParameter(0)->GetType());
					}
				}
				else if (_propertyInfo->IsWritable())
				{
					auto td = _propertyInfo->GetOwnerTypeDescriptor();
					if ((td->GetTypeDescriptorFlags() & TypeDescriptorFlags::ReferenceType) != TypeDescriptorFlags::Undefined)
					{
						setterType = CopyTypeInfo(_propertyInfo->GetReturn());
					}
				}

				ResolveExpressionResult result;
				result.propertyInfo = _propertyInfo;
				result.type = getterType;
				result.writableType = setterType;
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::Method(reflection::description::IMethodInfo* _methodInfo)
			{
				ResolveExpressionResult result;
				result.methodInfo = _methodInfo;
				result.type = CreateTypeInfoFromMethodInfo(_methodInfo);
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::Constructor(reflection::description::IMethodInfo* _constructorInfo)
			{
				ResolveExpressionResult result;
				result.constructorInfo = _constructorInfo;
				result.type = CopyTypeInfo(_constructorInfo->GetReturn());
				return result;
			}

			ResolveExpressionResult ResolveExpressionResult::Event(reflection::description::IEventInfo* _eventInfo)
			{
				ResolveExpressionResult result;
				result.eventInfo = _eventInfo;
				return result;
			}

			bool IsSameTypeOrNull(ITypeInfo* fromType, ITypeInfo* toType)
			{
				if (fromType == toType) return true;
				if ((fromType == nullptr) ^ (toType == nullptr)) return false;
				return IsSameType(fromType, toType);
			}

			bool ResolveExpressionResult::operator==(const ResolveExpressionResult& result) const
			{
				if (scopeName != result.scopeName) return false;
				if (symbol != result.symbol) return false;
				if (propertyInfo != result.propertyInfo) return false;
				if (methodInfo != result.methodInfo) return false;
				if (constructorInfo != result.constructorInfo) return false;
				if (eventInfo != result.eventInfo) return false;

				if (!IsSameTypeOrNull(type.Obj(), result.type.Obj())) return false;
				if (!IsSameTypeOrNull(writableType.Obj(), result.writableType.Obj())) return false;
				if (!IsSameTypeOrNull(expectedType.Obj(), result.expectedType.Obj())) return false;
				return true;
			}

			bool ResolveExpressionResult::operator!=(const ResolveExpressionResult& result) const
			{
				return !(*this == result);
			}

/***********************************************************************
WfLexicalScopeManager
***********************************************************************/

			WfLexicalScopeManager::WfLexicalScopeManager(Ptr<parsing::tabling::ParsingTable> _parsingTable)
				:parsingTable(_parsingTable)
			{
				attributes.Add({ L"cpp", L"File" }, TypeInfoRetriver<WString>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"UserImpl" }, TypeInfoRetriver<void>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"Private" }, TypeInfoRetriver<void>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"Protected" }, TypeInfoRetriver<void>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"Friend" }, TypeInfoRetriver<ITypeDescriptor*>::CreateTypeInfo());
			}

			WfLexicalScopeManager::~WfLexicalScopeManager()
			{
			}

			vint WfLexicalScopeManager::AddModule(const WString& moduleCode)
			{
				if (auto module = WfParseModule(moduleCode, parsingTable, errors, usedCodeIndex))
				{
					modules.Add(module);
					moduleCodes.Add(moduleCode);
				}
				return usedCodeIndex++;
			}

			vint WfLexicalScopeManager::AddModule(Ptr<WfModule> module)
			{
				module->codeRange.codeIndex = usedCodeIndex;
				modules.Add(module);
				moduleCodes.Add(L"");
				return usedCodeIndex++;
			}

			WfLexicalScopeManager::ModuleList& WfLexicalScopeManager::GetModules()
			{
				return modules;
			}

			WfLexicalScopeManager::ModuleCodeList& WfLexicalScopeManager::GetModuleCodes()
			{
				return moduleCodes;
			}

			void WfLexicalScopeManager::Clear(bool keepTypeDescriptorNames, bool deleteModules)
			{
				if (globalName)
				{
					if (keepTypeDescriptorNames)
					{
						globalName->RemoveNonTypeDescriptorNames(this);
					}
					else
					{
						globalName = 0;
						typeNames.Clear();
					}
				}
				
				if (deleteModules)
				{
					modules.Clear();
					moduleCodes.Clear();
					usedCodeIndex = 0;
				}

				errors.Clear();
				namespaceNames.Clear();

				nodeScopes.Clear();
				expressionResolvings.Clear();
				lambdaCaptures.Clear();
				interfaceMethodImpls.Clear();
				declarationTypes.Clear();
				declarationMemberInfos.Clear();
				baseConstructorCallResolvings.Clear();

				attributeValues.Clear();
			}

			void WfLexicalScopeManager::Rebuild(bool keepTypeDescriptorNames)
			{
				Clear(keepTypeDescriptorNames, false);
				if (!globalName)
				{
					globalName = new WfLexicalScopeName(true);
					BuildGlobalNameFromTypeDescriptors(this);
				}

				vint errorCount = errors.Count();

#define EXIT_IF_ERRORS_EXIST\
				do\
				{\
					if (errors.Count() != errorCount) return;\
				}while (0)
				
				EXIT_IF_ERRORS_EXIST;
				FOREACH(Ptr<WfModule>, module, modules)
				{
					ValidateModuleStructure(this, module);
				}
				
				EXIT_IF_ERRORS_EXIST;
				BuildGlobalNameFromModules(this);
				FOREACH(Ptr<WfModule>, module, modules)
				{
					BuildScopeForModule(this, module);
				}
				ValidateScopeName(this, globalName);
				CheckScopes_DuplicatedSymbol(this);
				
				EXIT_IF_ERRORS_EXIST;
				FOREACH(Ptr<WfModule>, module, modules)
				{
					CompleteScopeForModule(this, module);
				}
				CheckScopes_BaseType(this);
				CheckScopes_SymbolType(this);
				
				EXIT_IF_ERRORS_EXIST;
				FOREACH(Ptr<WfModule>, module, modules)
				{
					ValidateModuleSemantic(this, module);
				}

#undef EXIT_IF_ERRORS_EXIST
			}

			bool WfLexicalScopeManager::ResolveMember(ITypeDescriptor* typeDescriptor, const WString& name, bool preferStatic, collections::SortedList<ITypeDescriptor*>& searchedTypes, collections::List<ResolveExpressionResult>& results)
			{
				if (searchedTypes.Contains(typeDescriptor))
				{
					return true;
				}
				searchedTypes.Add(typeDescriptor);

				bool found = false;
				bool foundStaticMember = false;


				if ((typeDescriptor->GetTypeDescriptorFlags() & TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
				{
					if (preferStatic)
					{
						auto enumType = typeDescriptor->GetEnumType();
						vint index = enumType->IndexOfItem(name);
						if (index != -1)
						{
							results.Add(ResolveExpressionResult::ReadonlyType(MakePtr<TypeDescriptorTypeInfo>(typeDescriptor, TypeInfoHint::Normal)));
							return true;
						}
					}
					return false;
				}

				{
					auto scopeName = typeNames[typeDescriptor];
					vint index = scopeName->children.Keys().IndexOf(name);
					if (index != -1)
					{
						auto subScopeName = scopeName->children.Values()[index];
						found = true;
						foundStaticMember = true;
						results.Add(ResolveExpressionResult::ScopeName(subScopeName));
					}
				}

				if (auto group = typeDescriptor->GetMethodGroupByName(name, false))
				{
					for (vint i = 0; i < group->GetMethodCount(); i++)
					{
						auto info = group->GetMethod(i);
						if (info->IsStatic())
						{
							found = true;
							foundStaticMember = true;
							results.Add(ResolveExpressionResult::Method(info));
						}
					}
				}

				if (foundStaticMember && preferStatic)
				{
					return true;
				}

				if (auto group = typeDescriptor->GetMethodGroupByName(name, false))
				{
					for (vint i = 0; i < group->GetMethodCount(); i++)
					{
						auto info = group->GetMethod(i);
						if (!info->IsStatic())
						{
							found = true;
							results.Add(ResolveExpressionResult::Method(info));
						}
					}
				}
				if (auto info = typeDescriptor->GetPropertyByName(name, false))
				{
					found = true;
					results.Add(ResolveExpressionResult::Property(info));
				}
				if (auto info = typeDescriptor->GetEventByName(name, false))
				{
					found = true;
					results.Add(ResolveExpressionResult::Event(info));
				}

				if (!found)
				{
					vint count = typeDescriptor->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						if (ResolveMember(typeDescriptor->GetBaseTypeDescriptor(i), name, preferStatic, searchedTypes, results))
						{
							found = true;
						}
					}
				}
				return found;
			}

			class UsingPathToNameVisitor :public Object, public WfModuleUsingFragment::IVisitor
			{
			public:
				WString					name;
				WString					result;

				UsingPathToNameVisitor(const WString& _name)
					:name(_name)
				{
				}

				void Visit(WfModuleUsingNameFragment* node)
				{
					result = node->name.value;
				}

				void Visit(WfModuleUsingWildCardFragment* node)
				{
					result = name;
				}

				static WString Execute(Ptr<WfModuleUsingFragment> fragment, const WString& name)
				{
					UsingPathToNameVisitor visitor(name);
					fragment->Accept(&visitor);
					return visitor.result;
				}
			};

			bool WfLexicalScopeManager::ResolveName(WfLexicalScope* scope, const WString& name, collections::List<ResolveExpressionResult>& results)
			{
				vint oldResultCount = results.Count();
				bool visibleToNonStatic = false;
				WfLexicalScope* firstConfigScope = nullptr;
				while (scope)
				{
					if (scope->ownerNode.Cast<WfModule>() || scope->ownerNode.Cast<WfNamespaceDeclaration>())
					{
						break;
					}

					if (scope->functionConfig)
					{
						visibleToNonStatic = scope->functionConfig->thisAccessable || scope->functionConfig->parentThisAccessable;
						if (!firstConfigScope)
						{
							firstConfigScope = scope;
						}
					}
					
					vint index = scope->symbols.Keys().IndexOf(name);
					if (index != -1)
					{
						if (scope->typeOfThisExpr)
						{
							if (scope->ownerNode.Cast<WfNewInterfaceExpression>())
							{
								FOREACH(Ptr<WfLexicalSymbol>, symbol, scope->symbols.GetByIndex(index))
								{
									if (symbol->creatorNode.Cast<WfVariableDeclaration>())
									{
										auto result = ResolveExpressionResult::Symbol(symbol);
										if (!results.Contains(result))
										{
											results.Add(result);
										}
									}
									else if (symbol->creatorClassMember->kind == WfClassMemberKind::Normal)
									{
										if (firstConfigScope->parentScope == scope)
										{
											auto result = ResolveExpressionResult::Symbol(symbol);
											if (!results.Contains(result))
											{
												results.Add(result);
											}
										}
									}
								}
							}
						}
						else
						{
							FOREACH(Ptr<WfLexicalSymbol>, symbol, scope->symbols.GetByIndex(index))
							{
								auto result = ResolveExpressionResult::Symbol(symbol);
								if (!results.Contains(result))
								{
									results.Add(result);
								}
							}
						}
					}

					if (scope->typeOfThisExpr)
					{
						SortedList<ITypeDescriptor*> searchedTypes;
						ResolveMember(scope->typeOfThisExpr, name, !visibleToNonStatic, searchedTypes, results);
					}

					scope = scope->parentScope.Obj();
				}
				
				while (scope)
				{
					if (auto nsDecl = scope->ownerNode.Cast<WfNamespaceDeclaration>())
					{
						auto scopeName = namespaceNames[nsDecl.Obj()];
						vint index = scopeName->children.Keys().IndexOf(name);
						if (index != -1)
						{
							auto subScopeName = scopeName->children.Values()[index];
							auto result = ResolveExpressionResult::ScopeName(subScopeName);
							if (!results.Contains(result))
							{
								results.Add(result);
							}
						}
						scope = scope->parentScope.Obj();
					}
					else
					{
						break;
					}
				}
				
				vint index = globalName->children.Keys().IndexOf(name);
				if (index != -1)
				{
					auto subScopeName = globalName->children.Values()[index];
					auto result = ResolveExpressionResult::ScopeName(subScopeName);
					if (!results.Contains(result))
					{
						results.Add(result);
					}
				}

				if (auto module = scope->ownerNode.Cast<WfModule>())
				{
					FOREACH(Ptr<WfModuleUsingPath>, path, module->paths)
					{
						auto scopeName = globalName;
						FOREACH(Ptr<WfModuleUsingItem>, item, path->items)
						{
							WString fragmentName;
							FOREACH(Ptr<WfModuleUsingFragment>, fragment, item->fragments)
							{
								fragmentName += UsingPathToNameVisitor::Execute(fragment, name);
							}
							vint index = scopeName->children.Keys().IndexOf(fragmentName);
							if (index == -1) goto USING_PATH_MATCHING_FAILED;
							scopeName = scopeName->children.Values()[index];
						}

						{
							auto result = ResolveExpressionResult::ScopeName(scopeName);
							if (!results.Contains(result))
							{
								results.Add(result);
							}
						}
					USING_PATH_MATCHING_FAILED:;
					}
				}

				return results.Count() > oldResultCount;
			}

			Ptr<WfLexicalSymbol> WfLexicalScopeManager::GetDeclarationSymbol(WfLexicalScope* scope, WfDeclaration* node)
			{
				auto symbol = From(scope->parentScope->symbols[node->name.value])
					.Where([=](Ptr<WfLexicalSymbol> symbol)
					{
						return symbol->creatorNode == node;
					})
					.First();
				return symbol;
			}

			void WfLexicalScopeManager::CreateLambdaCapture(parsing::ParsingTreeCustomBase* node, Ptr<WfLexicalCapture> capture)
			{
				if (!capture)
				{
					capture = MakePtr<WfLexicalCapture>();
				}
				lambdaCaptures.Add(node, capture);
			}

			Ptr<WfAttribute> WfLexicalScopeManager::GetAttribute(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name)
			{
				return From(GetAttributes(atts, category, name)).First(nullptr);
			}

			collections::LazyList<Ptr<WfAttribute>> WfLexicalScopeManager::GetAttributes(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name)
			{
				return From(atts)
					.Where([=](Ptr<WfAttribute> att)
					{
						return att->category.value == category && att->name.value == name;
					});
			}

			Value WfLexicalScopeManager::GetAttributeValue(Ptr<WfAttribute> att)
			{
				if (!att->value)
				{
					return Value();
				}

				{
					vint index = attributeValues.Keys().IndexOf(att.Obj());
					if (index != -1)
					{
						return attributeValues.Values()[index];
					}
				}

				if (!attributeAssembly)
				{
					attributeAssembly = MakePtr<WfAssembly>();

					auto func = MakePtr<WfAssemblyFunction>();
					func->name = L"<get-attribute-value>";
					func->firstInstruction = 0;
					
					vint index = attributeAssembly->functions.Add(func);
					attributeAssembly->functionByName.Add(func->name, index);
				}

				attributeAssembly->insBeforeCodegen = MakePtr<WfInstructionDebugInfo>();
				attributeAssembly->insAfterCodegen = MakePtr<WfInstructionDebugInfo>();
				attributeAssembly->instructions.Clear();

				WfCodegenContext context(attributeAssembly, this);
				{
					auto recorderBefore = new ParsingGeneratedLocationRecorder(context.nodePositionsBeforeCodegen);
					auto recorderAfter = new ParsingGeneratedLocationRecorder(context.nodePositionsAfterCodegen);
					auto recorderOriginal = new ParsingOriginalLocationRecorder(recorderBefore);
					auto recorderMultiple = new ParsingMultiplePrintNodeRecorder;
					recorderMultiple->AddRecorder(recorderOriginal);
					recorderMultiple->AddRecorder(recorderAfter);

					stream::MemoryStream memoryStream;
					{
						stream::StreamWriter streamWriter(memoryStream);
						ParsingWriter parsingWriter(streamWriter, recorderMultiple);
						WfPrint(att->value, L"", parsingWriter);
					}
				}
				auto typeInfo = attributes[{att->category.value, att->name.value}];
				GenerateExpressionInstructions(context, att->value, typeInfo);
				attributeAssembly->instructions.Add(WfInstruction::Return());

				if (!attributeGlobalContext)
				{
					attributeGlobalContext = MakePtr<WfRuntimeGlobalContext>(attributeAssembly);
				}
				auto func = LoadFunction<Value()>(attributeGlobalContext, L"<get-attribute-value>");
				auto value = func();
				attributeValues.Add(att, value);
				return func();
			}

/***********************************************************************
WfCodegenFunctionContext
***********************************************************************/

			WfCodegenFunctionContext::WfCodegenFunctionContext()
			{
				scopeContextStack.Add(new WfCodegenScopeContext);
			}

			Ptr<WfCodegenScopeContext> WfCodegenFunctionContext::GetCurrentScopeContext()
			{
				return scopeContextStack[scopeContextStack.Count() - 1];
			}

			Ptr<WfCodegenScopeContext> WfCodegenFunctionContext::GetCurrentScopeContext(WfCodegenScopeType type)
			{
				for (vint i = scopeContextStack.Count() - 1; i >= 0; i--)
				{
					auto context = scopeContextStack[i];
					if (context->type == type)
					{
						return context;
					}
				}
				return 0;
			}

			Ptr<WfCodegenScopeContext> WfCodegenFunctionContext::PushScopeContext(WfCodegenScopeType type)
			{
				auto context = MakePtr<WfCodegenScopeContext>();
				context->type = type;
				scopeContextStack.Add(context);
				return context;
			}

			void WfCodegenFunctionContext::PopScopeContext()
			{
				scopeContextStack.RemoveAt(scopeContextStack.Count() - 1);
			}

/***********************************************************************
WfCodegenContext
***********************************************************************/

			WfCodegenContext::WfCodegenContext(Ptr<runtime::WfAssembly> _assembly, WfLexicalScopeManager* _manager)
				:assembly(_assembly)
				, manager(_manager)
			{
			}

			vint WfCodegenContext::AddInstruction(parsing::ParsingTreeCustomBase* node, const runtime::WfInstruction& ins)
			{
				auto index = assembly->instructions.Add(ins);
				if (node)
				{
					assembly->insBeforeCodegen->instructionCodeMapping.Add(nodePositionsBeforeCodegen[node]);
					assembly->insAfterCodegen->instructionCodeMapping.Add(nodePositionsAfterCodegen[node]);
				}
				else
				{
					parsing::ParsingTextRange range;
					assembly->insBeforeCodegen->instructionCodeMapping.Add(range);
					assembly->insAfterCodegen->instructionCodeMapping.Add(range);
				}
				return index;
			}

			void WfCodegenContext::AddExitInstruction(parsing::ParsingTreeCustomBase* node, const runtime::WfInstruction& ins)
			{
				auto context = functionContext->GetCurrentScopeContext();
				context->exitInstructions.Add(ins);
				context->instructionCodeMappingBeforeCodegen.Add(nodePositionsBeforeCodegen[node]);
				context->instructionCodeMappingAfterCodegen.Add(nodePositionsAfterCodegen[node]);
			}

			void WfCodegenContext::ApplyExitInstructions(Ptr<WfCodegenScopeContext> scopeContext)
			{
				if (scopeContext->exitInstructions.Count() > 0)
				{
					CopyFrom(assembly->instructions, scopeContext->exitInstructions, true);
					CopyFrom(assembly->insBeforeCodegen->instructionCodeMapping, scopeContext->instructionCodeMappingBeforeCodegen, true);
					CopyFrom(assembly->insAfterCodegen->instructionCodeMapping, scopeContext->instructionCodeMappingAfterCodegen, true);
				}
			}

			vint WfCodegenContext::GetThisStackCount(WfLexicalScope* scope)
			{
				vint count = 0;
				Ptr<WfLexicalFunctionConfig> firstConfig, methodConfig;
				while (scope)
				{
					if (scope->typeOfThisExpr)
					{
						if (methodConfig)
						{
							count++;
							if (!methodConfig->parentThisAccessable)
							{
								break;
							}
							methodConfig = nullptr;
						}
					}

					if (scope->functionConfig)
					{
						if (!firstConfig)
						{
							vint index = thisStackCounts.Keys().IndexOf(scope->functionConfig.Obj());
							if (index == -1)
							{
								firstConfig = scope->functionConfig;
							}
							else
							{
								return thisStackCounts.Values()[index];
							}
						}
						if (scope->functionConfig->thisAccessable)
						{
							methodConfig = scope->functionConfig;
						}
					}
					scope = scope->parentScope.Obj();
				}

				if (firstConfig)
				{
					thisStackCounts.Add(firstConfig, count);
				}
				return count;
			}
		}
	}
}