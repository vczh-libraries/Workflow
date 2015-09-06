#include "WfRuntime.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			using namespace reflection::description;

/***********************************************************************
WfRuntimeReverseEnumerable
***********************************************************************/

			WfRuntimeReverseEnumerable::Enumerator::Enumerator(Ptr<reflection::description::IValueList> _list)
				:list(_list), index(_list->GetCount())
			{
			}

			reflection::description::Value WfRuntimeReverseEnumerable::Enumerator::GetCurrent()
			{
				return list->Get(index);
			}

			vint WfRuntimeReverseEnumerable::Enumerator::GetIndex()
			{
				return list->GetCount() - 1 - index;
			}

			bool WfRuntimeReverseEnumerable::Enumerator::Next()
			{
				if (index <= 0) return false;
				index--;
				return true;
			}

			WfRuntimeReverseEnumerable::WfRuntimeReverseEnumerable(Ptr<reflection::description::IValueList> _list)
				:list(_list)
			{
			}

			Ptr<reflection::description::IValueEnumerator> WfRuntimeReverseEnumerable::CreateEnumerator()
			{
				return MakePtr<Enumerator>(list);
			}
			
/***********************************************************************
WfRuntimeLambda
***********************************************************************/

			WfRuntimeLambda::WfRuntimeLambda(Ptr<WfRuntimeGlobalContext> _globalContext, Ptr<WfRuntimeVariableContext> _capturedVariables, vint _functionIndex)
				:globalContext(_globalContext)
				, capturedVariables(_capturedVariables)
				, functionIndex(_functionIndex)
			{
			}

			reflection::description::Value WfRuntimeLambda::Invoke(Ptr<reflection::description::IValueList> arguments)
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

			reflection::description::Value WfRuntimeInterfaceInstance::Invoke(const WString& name, Ptr<reflection::description::IValueList> arguments)
			{
				vint index = functions.Keys().IndexOf(name);
				if (index == -1)
				{
					throw WfRuntimeException(L"Internal error: failed to invoke the interface method \"" + name + L"\"", true);
				}
				else
				{
					return functions.Values()[index]->Invoke(arguments);
				}
			}
		}
	}
}