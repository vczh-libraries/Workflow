/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Analyzer::RPC Generator Helpers

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_ANALYZER_WFANALYZER_GENERATERPC
#define VCZH_WORKFLOW_ANALYZER_WFANALYZER_GENERATERPC

#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			namespace rpc_generating
			{
				extern Ptr<WfStatement>											CreateIf(Ptr<WfExpression> condition, Ptr<WfStatement> trueBranch, Ptr<WfStatement> falseBranch = nullptr);
				extern Ptr<WfFunctionDeclaration>								CreateFunctionDeclaration(const WString& name, Ptr<WfType> returnType, WfFunctionKind kind, WfFunctionAnonymity anonymity = WfFunctionAnonymity::Named);

				template<typename ...TArgs>
				Ptr<WfCallExpression> CreateCall(Ptr<WfExpression> function, TArgs... arguments)
				{
					auto expression = Ptr(new WfCallExpression);
					expression->function = function;
					if constexpr (sizeof...(arguments) > 0)
					{
						(expression->arguments.Add(arguments), ...);
					}
					return expression;
				}
			}
		}
	}
}

#endif
