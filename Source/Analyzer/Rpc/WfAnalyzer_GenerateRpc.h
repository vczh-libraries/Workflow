/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Analyzer::RPC Generator Helpers

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_ANALYZER_RPC_WFANALYZER_GENERATERPC
#define VCZH_WORKFLOW_ANALYZER_RPC_WFANALYZER_GENERATERPC

#include "WfAnalyzer_RpcAstHelpers.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			namespace rpc_generating
			{
				extern bool														IsSharedInterfaceType(reflection::description::ITypeInfo* type);
				extern bool														IsVoidType(WfType* type);
				extern bool														IsRpcByvalReturn(const RpcMethodModel& methodModel);
				extern WString													MangleRpcFullName(const WString& fullName);
				extern void														CollectMangledNames(WfLexicalScopeManager* manager);
				extern collections::List<RpcInterfaceModel>						BuildInterfaceModels(WfLexicalScopeManager* manager);
				extern bool														HasRpcEvents(const collections::List<RpcInterfaceModel>& interfaces);
				extern WString													GetRpcOpsInterfaceName(const WString& assemblyName);
				extern WString													GetRpcOpsInvokeMethodName(const RpcMethodModel& methodModel);
				extern WString													GetRpcOpsInvokeEventName(const RpcEventModel& eventModel);
				extern WString													GetRpcOpsArgumentName(const RpcParamModel& paramModel);
				extern Ptr<WfFunctionDeclaration>								CreateRpcOpsFunctionDeclaration(const WString& name, Ptr<WfType> returnType, WfFunctionKind kind);
				extern void														AddRpcOpsFunctionArguments(Ptr<WfFunctionDeclaration> functionDecl, const collections::List<RpcParamModel>& params);
				extern Ptr<WfExpression>										CreateRpcBoxExpression(reflection::description::ITypeInfo* typeInfo, bool byref, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle);
				extern Ptr<WfExpression>										CreateRpcUnboxExpression(reflection::description::ITypeInfo* typeInfo, Ptr<WfType> type, bool byref, Ptr<WfExpression> value, Ptr<WfExpression> lifecycle);
				extern Ptr<WfExpression>										CreateRpcCopyByvalExpression(Ptr<WfExpression> value, Ptr<WfExpression> lifecycle);
				extern void														AddRpcByvalReturnValue(Ptr<WfBlockStatement> block, Ptr<WfExpression> value, Ptr<WfExpression> copiedValue);
				extern Ptr<WfExpression>										CreateRpcConstantReference(const wchar_t* prefix, const WString& fullName);
				extern Ptr<WfExpression>										CreateRpcOpsObjectOps();
				extern Ptr<WfExpression>										CreateRpcOpsObjectInvoke(const RpcMethodModel& methodModel, Ptr<WfExpression> objectOps);
				extern Ptr<WfExpression>										CreateRpcOpsObjectInvoke(const RpcMethodModel& methodModel);
				extern Ptr<WfExpression>										CreateRpcOpsObjectEventInvoke(const RpcEventModel& eventModel);
			}
		}
	}
}

#endif
