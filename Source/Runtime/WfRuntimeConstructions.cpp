#include "WfRuntime.h"

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			using namespace reflection::description;
			
/***********************************************************************
WfRuntimeLambda
***********************************************************************/

			WfRuntimeLambda::WfRuntimeLambda(Ptr<WfRuntimeGlobalContext> _globalContext, Ptr<WfRuntimeVariableContext> _capturedVariables, vint _functionIndex)
				:globalContext(_globalContext)
				, capturedVariables(_capturedVariables)
				, functionIndex(_functionIndex)
			{
			}

			Value WfRuntimeLambda::Invoke(Ptr<reflection::description::IValueReadonlyList> arguments)
			{
				return Invoke(globalContext, capturedVariables, functionIndex, arguments);
			}

			Value WfRuntimeLambda::Invoke(Ptr<WfRuntimeGlobalContext> globalContext, Ptr<WfRuntimeVariableContext> capturedVariables, vint functionIndex, Ptr<reflection::description::IValueReadonlyList> arguments)
			{
				WfRuntimeThreadContext context(globalContext);
				vint count = arguments->GetCount();
				for (vint i = 0; i < count; i++)
				{
					context.PushValue(arguments->Get(i));
				}
					
				WString message;
				if (context.PushStackFrame(functionIndex, count, capturedVariables) != WfRuntimeThreadContextError::Success)
				{
					throw WfRuntimeException(L"Internal error: failed to invoke a function.", true);
				}

				context.ExecuteToEnd();
				if (context.status != WfRuntimeExecutionStatus::Finished)
				{
					throw WfRuntimeException(context.exceptionInfo);
				}

				Value result;
				if (context.PopValue(result) != WfRuntimeThreadContextError::Success)
				{
					throw WfRuntimeException(L"Internal error: failed to pop the function result.", true);
				}
					
				return result;
			}
			
/***********************************************************************
WfRuntimeInterfaceInstance
***********************************************************************/

			Value WfRuntimeInterfaceInstance::Invoke(IMethodInfo* methodInfo, Ptr<IValueReadonlyList> arguments)
			{
				vint index = functions.Keys().IndexOf(methodInfo);
				if (index == -1)
				{
					throw WfRuntimeException(
						L"Internal error: failed to invoke the interface method \"" +
						methodInfo->GetName() +
						L"\" of type \"" +
						methodInfo->GetOwnerTypeDescriptor()->GetTypeName() +
						L"\"", true);
				}
				else
				{
					vint functionIndex = functions.Values()[index];
					return WfRuntimeLambda::Invoke(globalContext, capturedVariables, functionIndex, arguments);
				}
			}
		}
	}
}

#endif