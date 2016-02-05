/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIMECONSTRUCTIONS
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIMECONSTRUCTIONS

#include "WfRuntimeAssembly.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			class WfRuntimeGlobalContext;
			class WfRuntimeVariableContext;
			
/***********************************************************************
Range
***********************************************************************/

			template<typename T>
			class WfRuntimeRange : public Object, public reflection::description::IValueEnumerable
			{
			protected:
				T						begin;
				T						end;

				class Enumerator : public Object, public reflection::description::IValueEnumerator
				{
				protected:
					T					begin;
					T					end;
					T					current;
				public:
					Enumerator(T _begin, T _end)
						:begin(_begin), end(_end), current(_begin - 1)
					{
					}

					reflection::description::Value GetCurrent()
					{
						return reflection::description::BoxValue<T>(current);
					}

					vint GetIndex()
					{
						return (vint)(current - begin);
					}

					bool Next()
					{
						if (current >= end) return false;
						current++;
						return true;
					}
				};
			public:
				WfRuntimeRange(T _begin, T _end)
					:begin(_begin), end(_end)
				{
				}

				Ptr<reflection::description::IValueEnumerator> CreateEnumerator()override
				{
					return MakePtr<Enumerator>(begin, end);
				}
			};
			
/***********************************************************************
ReverseEnumerable
***********************************************************************/

			class WfRuntimeReverseEnumerable : public Object, public reflection::description::IValueEnumerable
			{
			protected:
				Ptr<reflection::description::IValueList>		list;

				class Enumerator : public Object, public reflection::description::IValueEnumerator
				{
				protected:
					Ptr<reflection::description::IValueList>	list;
					vint										index;

				public:
					Enumerator(Ptr<reflection::description::IValueList> _list);
					reflection::description::Value				GetCurrent();
					vint										GetIndex();
					bool										Next();
				};

			public:
				WfRuntimeReverseEnumerable(Ptr<reflection::description::IValueList> _list);

				Ptr<reflection::description::IValueEnumerator>	CreateEnumerator()override;
			};
			
/***********************************************************************
Lambda
***********************************************************************/

			class WfRuntimeLambda : public Object, public reflection::description::IValueFunctionProxy
			{
			public:
				Ptr<WfRuntimeGlobalContext>			globalContext;
				Ptr<WfRuntimeVariableContext>		capturedVariables;
				vint								functionIndex;

				WfRuntimeLambda(Ptr<WfRuntimeGlobalContext> _globalContext, Ptr<WfRuntimeVariableContext> _capturedVariables, vint _functionIndex);

				reflection::description::Value		Invoke(Ptr<reflection::description::IValueList> arguments)override;
			};
			
/***********************************************************************
InterfaceInstance
***********************************************************************/

			class WfRuntimeInterfaceInstance : public Object, public reflection::description::IValueInterfaceProxy
			{
				typedef reflection::description::IMethodInfo								IMethodInfo;
				typedef reflection::description::IValueFunctionProxy						IValueFunctionProxy;
				typedef reflection::description::IValueList									IValueList;
				typedef collections::Dictionary<IMethodInfo*, Ptr<IValueFunctionProxy>>		FunctionMap;
			public:
				FunctionMap							functions;

				reflection::description::Value		Invoke(IMethodInfo* methodInfo, Ptr<IValueList> arguments)override;
			};
		}
	}
}

#endif
