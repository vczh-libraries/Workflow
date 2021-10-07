#include "WfEmitter.h"

namespace vl
{
	namespace workflow
	{
		namespace emitter
		{
			using namespace parsing;
			using namespace reflection::description;
			using namespace analyzer;
			using namespace runtime;

/***********************************************************************
WfAttributeEvaluator
***********************************************************************/

			WfAttributeEvaluator::WfAttributeEvaluator(analyzer::WfLexicalScopeManager* _manager)
				:manager(_manager)
			{
			}

			Ptr<WfAttribute> WfAttributeEvaluator::GetAttribute(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name)
			{
				return From(GetAttributes(atts, category, name)).First(nullptr);
			}

			collections::LazyList<Ptr<WfAttribute>> WfAttributeEvaluator::GetAttributes(collections::List<Ptr<WfAttribute>>& atts, const WString& category, const WString& name)
			{
				return From(atts)
					.Where([=](Ptr<WfAttribute> att)
					{
						return att->category.value == category && att->name.value == name;
					});
			}

			runtime::WfRuntimeValue WfAttributeEvaluator::GetAttributeValue(Ptr<WfAttribute> att)
			{
				if (!att->value)
				{
					return {};
				}

				{
					vint index = attributeValues.Keys().IndexOf(att.Obj());
					if (index != -1)
					{
						return attributeValues.Values()[index];
					}
				}

				auto attributeAssembly = MakePtr<WfAssembly>();
				WfCodegenContext context(attributeAssembly, manager);
				auto typeInfo = manager->attributes[{att->category.value, att->name.value}];
				GenerateExpressionInstructions(context, att->value, typeInfo);

				CHECK_ERROR(attributeAssembly->instructions.Count() == 1, L"WfAttributeEvaluator::GetAttributeValue(Ptr<WfAttribute>)#Internal error, attribute argument generates unexpected instructions.");
				auto& ins = attributeAssembly->instructions[0];
				CHECK_ERROR(ins.code == WfInsCode::LoadValue, L"WfAttributeEvaluator::GetAttributeValue(Ptr<WfAttribute>)#Internal error, attribute argument generates unexpected instructions.");
				attributeValues.Add(att, ins.valueParameter);
				return ins.valueParameter;
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

			Ptr<WfCodegenScopeContext> WfCodegenFunctionContext::GetCurrentScopeContext(WfCodegenScopeType type, const WString& name)
			{
				for (vint i = scopeContextStack.Count() - 1; i >= 0; i--)
				{
					auto context = scopeContextStack[i];
					if (context->type == type)
					{
						if (name == L"" || context->name == name)
						{
							return context;
						}
					}
				}
				return 0;
			}

			Ptr<WfCodegenScopeContext> WfCodegenFunctionContext::PushScopeContext(WfCodegenScopeType type, const WString& name)
			{
				auto context = MakePtr<WfCodegenScopeContext>();
				context->type = type;
				context->name = name;
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

			WfCodegenContext::WfCodegenContext(Ptr<runtime::WfAssembly> _assembly, analyzer::WfLexicalScopeManager* _manager)
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

			vint WfCodegenContext::GetThisStackCount(analyzer::WfLexicalScope* scope)
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