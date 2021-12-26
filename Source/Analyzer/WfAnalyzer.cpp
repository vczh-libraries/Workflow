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

			WfLexicalScope* WfLexicalScope::FindFunctionScope()
			{
				auto scope = this;
				while (scope)
				{
					if (scope->functionConfig)
					{
						return scope;
					}
					scope = scope->parentScope.Obj();
				}
				return nullptr;
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
				for (auto name : children.Values())
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

				if (getterType)
				{
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

			WfLexicalScopeManager::WfLexicalScopeManager(workflow::Parser& _workflowParser)
				:workflowParser(_workflowParser)
			{
				workflowParserHandler = glr::InstallDefaultErrorMessageGenerator(workflowParser, errors);
				attributes.Add({ L"cpp", L"File" }, TypeInfoRetriver<WString>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"UserImpl" }, TypeInfoRetriver<void>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"Private" }, TypeInfoRetriver<void>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"Protected" }, TypeInfoRetriver<void>::CreateTypeInfo());
				attributes.Add({ L"cpp", L"Friend" }, TypeInfoRetriver<ITypeDescriptor*>::CreateTypeInfo());
			}

			WfLexicalScopeManager::~WfLexicalScopeManager()
			{
				workflowParser.OnError.Remove(workflowParserHandler);
			}

			vint WfLexicalScopeManager::AddModule(const WString& moduleCode)
			{
				if (auto module = ParseModule(moduleCode, workflowParser, usedCodeIndex))
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

				usedTempVars = 0;
				errors.Clear();
				namespaceNames.Clear();
				nodeScopes.Clear();
				checkedScopes_DuplicatedSymbol.Clear();
				checkedScopes_SymbolType.Clear();

				expressionResolvings.Clear();
				coNewCoroutineResolvings.Clear();
				coOperatorResolvings.Clear();
				coProviderResolvings.Clear();
				coCastResultResolvings.Clear();
				baseConstructorCallResolvings.Clear();

				lambdaCaptures.Clear();
				interfaceMethodImpls.Clear();
				declaractionScopeSources.Clear();
				declarationTypes.Clear();
				declarationMemberInfos.Clear();
				stateInputMethods.Clear();
				stateInputArguments.Clear();
				stateDeclArguments.Clear();
				stateMachineInfos.Clear();
			}

#define EXECUTE_CALLBACK(EXPR) if (callback) callback->EXPR

			void WfLexicalScopeManager::Rebuild(bool keepTypeDescriptorNames, IWfCompilerCallback* callback)
			{
				EXECUTE_CALLBACK(OnLoadEnvironment());
				Clear(keepTypeDescriptorNames, false);
				if (!globalName)
				{
					globalName = new WfLexicalScopeName(true);
					BuildGlobalNameFromTypeDescriptors(this);
				}

				EXECUTE_CALLBACK(OnInitialize(this));
				vint errorCount = errors.Count();

#define EXIT_IF_ERRORS_EXIST\
				do\
				{\
					if (errors.Count() != errorCount) return;\
				}while (0)
				
				EXIT_IF_ERRORS_EXIST;
				for (auto module : modules)
				{
					ContextFreeModuleDesugar(this, module);
					ValidateModuleStructure(this, module);
				}
				
				EXIT_IF_ERRORS_EXIST;
				BuildGlobalNameFromModules(this);
				for (auto module : modules)
				{
					BuildScopeForModule(this, module);
				}
				ValidateScopeName(this, globalName);
				CheckScopes_DuplicatedSymbol(this);
				
				EXIT_IF_ERRORS_EXIST;
				for (auto module : modules)
				{
					CompleteScopeForModule(this, module);
				}
				CheckScopes_SymbolType(this);
				CheckScopes_CycleDependency(this);
				
				EXIT_IF_ERRORS_EXIST;
				for (auto module : modules)
				{
					EXECUTE_CALLBACK(OnValidateModule(module));
					ValidateModuleSemantic(this, module);
				}

#undef EXIT_IF_ERRORS_EXIST
			}

#undef EXECUTE_CALLBACK

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
								for (auto symbol : scope->symbols.GetByIndex(index))
								{
									if (auto functionDecl = symbol->creatorNode.Cast<WfFunctionDeclaration>())
									{
										if (functionDecl->functionKind != WfFunctionKind::Normal)
										{
											continue;
										}
									}
									else if (auto autoPropDecl = symbol->creatorNode.Cast<WfAutoPropertyDeclaration>())
									{
										if (autoPropDecl->functionKind != WfFunctionKind::Normal)
										{
											continue;
										}
									}

									auto result = ResolveExpressionResult::Symbol(symbol);
									if (!results.Contains(result))
									{
										results.Add(result);
									}
								}
							}
						}
						else
						{
							for (auto symbol : scope->symbols.GetByIndex(index))
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
					for (auto path : module->paths)
					{
						auto scopeName = globalName;
						for (auto item : path->items)
						{
							WString fragmentName;
							for (auto fragment : item->fragments)
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

			void WfLexicalScopeManager::CreateLambdaCapture(glr::ParsingAstBase* node, Ptr<WfLexicalCapture> capture)
			{
				if (!capture)
				{
					capture = MakePtr<WfLexicalCapture>();
				}
				lambdaCaptures.Add(node, capture);
			}
		}
	}
}