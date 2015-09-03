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
					if (scope->ownerModule)
					{
						return scope->ownerModule;
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
					if (scope->ownerDeclaration)
					{
						return scope->ownerDeclaration;
					}
					scope = scope->parentScope.Obj();
				}
				return 0;
			}

			WString WfLexicalScope::GetFriendlyName()
			{
				if (ownerModule)
				{
					return L"<" + ownerModule->name.value + L">";
				}
				else if (ownerDeclaration)
				{
					return parentScope->GetFriendlyName() + L"::" + ownerDeclaration->name.value;
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

			WfLexicalScopeName::WfLexicalScopeName(bool _createdByTypeDescriptor)
				:parent(0)
				, createdByTypeDescriptor(_createdByTypeDescriptor)
				, typeDescriptor(0)
			{
			}

			WfLexicalScopeName::~WfLexicalScopeName()
			{
			}

			Ptr<WfLexicalScopeName> WfLexicalScopeName::AccessChild(const WString& name, bool createdByTypeDescriptor)
			{
				vint index = children.Keys().IndexOf(name);
				if (index == -1)
				{
					Ptr<WfLexicalScopeName> newName = new WfLexicalScopeName(createdByTypeDescriptor);
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

			void WfLexicalScopeName::RemoveNonTypeDescriptorNames()
			{
				for (vint i = children.Count() - 1; i >= 0; i--)
				{
					if (!children.Values()[i]->createdByTypeDescriptor)
					{
						children.Remove(children.Keys()[i]);
					}
				}
				
				declarations.Clear();
				FOREACH(Ptr<WfLexicalScopeName>, name, children.Values())
				{
					name->RemoveNonTypeDescriptorNames();
				}
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

			ResolveExpressionResult::ResolveExpressionResult()
				:propertyInfo(0)
				, methodInfo(0)
				, eventInfo(0)
			{
			}

			ResolveExpressionResult::ResolveExpressionResult(Ptr<WfLexicalScopeName> _scopeName)
				:scopeName(_scopeName)
				, propertyInfo(0)
				, methodInfo(0)
				, eventInfo(0)
			{
			}

			ResolveExpressionResult::ResolveExpressionResult(Ptr<reflection::description::ITypeInfo> _type, Ptr<reflection::description::ITypeInfo> _leftValueType)
				:type(_type)
				, leftValueType(_leftValueType)
				, propertyInfo(0)
				, methodInfo(0)
				, eventInfo(0)
			{
			}

			ResolveExpressionResult::ResolveExpressionResult(Ptr<WfLexicalSymbol> _symbol, Ptr<reflection::description::ITypeInfo> _type, Ptr<reflection::description::ITypeInfo> _leftValueType)
				:symbol(_symbol)
				, type(_type)
				, leftValueType(_leftValueType)
				, propertyInfo(0)
				, methodInfo(0)
				, eventInfo(0)
			{
			}

			ResolveExpressionResult::ResolveExpressionResult(reflection::description::IPropertyInfo* _propertyInfo, Ptr<reflection::description::ITypeInfo> _type, Ptr<reflection::description::ITypeInfo> _leftValueType)
				:propertyInfo(_propertyInfo)
				, methodInfo(0)
				, eventInfo(0)
				, type(_type)
				, leftValueType(_leftValueType)
			{
			}

			ResolveExpressionResult::ResolveExpressionResult(reflection::description::IMethodInfo* _methodInfo, Ptr<reflection::description::ITypeInfo> _type)
				:propertyInfo(0)
				, methodInfo(_methodInfo)
				, eventInfo(0)
				, type(_type)
			{
			}

			ResolveExpressionResult::ResolveExpressionResult(reflection::description::IEventInfo* _eventInfo)
				:propertyInfo(0)
				, methodInfo(0)
				, eventInfo(_eventInfo)
			{
			}

			WString ResolveExpressionResult::GetFriendlyName()const
			{
				WString typeName;
				if (type)
				{
					typeName= + L" of type \"" + type->GetTypeFriendlyName() + L"\"";
				}
				if (scopeName)
				{
					return scopeName->GetFriendlyName();
				}
				else if (symbol)
				{
					return symbol->GetFriendlyName() + typeName;
				}
				else if (propertyInfo)
				{
					return L"property \"" + propertyInfo->GetName() + L"\" in \"" + propertyInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"" + typeName;
				}
				else if (methodInfo)
				{
					if (methodInfo->GetName() == L"")
					{
						return L"constructor in " + methodInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"" + typeName;
					}
					else
					{
						return L"method \"" + methodInfo->GetName() + L"\" in \"" + methodInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"" + typeName;
					}
				}
				else if (eventInfo)
				{
					return L"event \"" + eventInfo->GetName() + L"\" in \"" + eventInfo->GetOwnerTypeDescriptor()->GetTypeName() + L"\"";
				}
				else if (type)
				{
					return L"expression" + typeName;
				}
				else
				{
					return L"<unknown>";
				}
			}

/***********************************************************************
WfLexicalScopeManager
***********************************************************************/

			WfLexicalScopeManager::WfLexicalScopeManager(Ptr<parsing::tabling::ParsingTable> _parsingTable)
				:parsingTable(_parsingTable)
			{
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
						globalName->RemoveNonTypeDescriptorNames();
					}
					else
					{
						globalName = 0;
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
				analyzedScopes.Clear();

				moduleScopes.Clear();
				declarationScopes.Clear();
				statementScopes.Clear();
				expressionScopes.Clear();
				expressionResolvings.Clear();
				functionLambdaCaptures.Clear();
				orderedLambdaCaptures.Clear();

				declarationTypes.Clear();
				declarationMemberInfos.Clear();
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
				CheckScopes(this);
				
				EXIT_IF_ERRORS_EXIST;
				FOREACH(Ptr<WfModule>, module, modules)
				{
					CompleteScopeForModule(this, module);
				}
				
				EXIT_IF_ERRORS_EXIST;
				FOREACH(Ptr<WfModule>, module, modules)
				{
					ValidateModuleSemantic(this, module);
				}

#undef EXIT_IF_ERRORS_EXIST
			}
			
			void WfLexicalScopeManager::ResolveSymbol(WfLexicalScope* scope, const WString& symbolName, collections::List<Ptr<WfLexicalSymbol>>& symbols)
			{
				while (scope && !scope->ownerModule && !scope->ownerDeclaration.Cast<WfNamespaceDeclaration>())
				{
					vint index = scope->symbols.Keys().IndexOf(symbolName);
					if (index != -1)
					{
						CopyFrom(symbols, scope->symbols.GetByIndex(index), true);
					}
					scope = scope->parentScope.Obj();
				}
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

			void WfLexicalScopeManager::ResolveScopeName(WfLexicalScope* scope, const WString& symbolName, collections::List<Ptr<WfLexicalScopeName>>& names)
			{
				while (scope)
				{
					if (scope->ownerModule || scope->ownerDeclaration.Cast<WfNamespaceDeclaration>())
					{
						break;
					}
					scope = scope->parentScope.Obj();
				}

				List<WString> namespacePath;
				Ptr<WfModule> module;
				while (scope)
				{
					if (auto ns = scope->ownerDeclaration.Cast<WfNamespaceDeclaration>())
					{
						namespacePath.Add(ns->name.value);
					}
					if (!module)
					{
						module = scope->ownerModule;
					}
					scope = scope->parentScope.Obj();
				}

				Ptr<WfLexicalScopeName> scopeName = globalName;
				vint nsIndex = namespacePath.Count();
				while (scopeName)
				{
					vint index = scopeName->children.Keys().IndexOf(symbolName);
					if (index != -1)
					{
						names.Add(scopeName->children.Values()[index]);
					}

					if (--nsIndex < 0) break;
					index = scopeName->children.Keys().IndexOf(namespacePath[nsIndex]);
					if (index == -1) break;
					scopeName = scopeName->children.Values()[index];
				}

				FOREACH(Ptr<WfModuleUsingPath>, path, module->paths)
				{
					scopeName = globalName;
					FOREACH(Ptr<WfModuleUsingItem>, item, path->items)
					{
						WString name;
						FOREACH(Ptr<WfModuleUsingFragment>, fragment, item->fragments)
						{
							name += UsingPathToNameVisitor::Execute(fragment, symbolName);
						}
						vint index = scopeName->children.Keys().IndexOf(name);
						if (index == -1) goto USING_PATH_MATCHING_FAILED;
						scopeName = scopeName->children.Values()[index];
					}
					names.Add(scopeName);
				USING_PATH_MATCHING_FAILED:;
				}
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
		}
	}
}