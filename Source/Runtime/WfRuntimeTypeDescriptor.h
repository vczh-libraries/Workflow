/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIMETYPEDESCRIPTOR
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIMETYPEDESCRIPTOR

#include "../WorkflowVlppReferences.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			class WfRuntimeGlobalContext;
		}

		namespace typeimpl
		{
			class WfCustomType;
			class WfTypeImpl;

/***********************************************************************
Method
***********************************************************************/

			class WfMethodBase : public reflection::description::MethodInfoImpl
			{
				friend class WfCustomType;
				typedef reflection::description::ITypeInfo					ITypeInfo;
			protected:
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;
				
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
			public:
				WfMethodBase(bool isStatic);
				~WfMethodBase();

				runtime::WfRuntimeGlobalContext*		GetGlobalContext();
				void									SetReturn(Ptr<ITypeInfo> type);
			};

			class WfStaticMethod : public WfMethodBase
			{
				typedef reflection::description::Value						Value;
			protected:

				Value									InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override;
				Value									CreateFunctionProxyInternal(const Value& thisObject)override;
			public:
				vint									functionIndex = -1;

				WfStaticMethod();
			};

			class WfInterfaceMethod : public WfMethodBase
			{
				typedef reflection::description::Value						Value;
			protected:

				Value									InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override;
				Value									CreateFunctionProxyInternal(const Value& thisObject)override;
			public:
				WfInterfaceMethod();
			};

/***********************************************************************
Event
***********************************************************************/

			class WfEvent : public reflection::description::EventInfoImpl
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::ITypeInfo					ITypeInfo;
				typedef reflection::description::IEventHandler				IEventHandler;
				typedef reflection::description::Value						Value;
			protected:

				void									AttachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)override;
				void									DetachInternal(DescriptableObject* thisObject, IEventHandler* eventHandler)override;
				void									InvokeInternal(DescriptableObject* thisObject, collections::Array<Value>& arguments)override;
				Ptr<ITypeInfo>							GetHandlerTypeInternal()override;
			public:
				WfEvent(ITypeDescriptor* ownerTypeDescriptor, const WString& name);
				~WfEvent();

				void									SetHandlerType(Ptr<ITypeInfo> typeInfo);
			};

/***********************************************************************
Property
***********************************************************************/

			class WfProperty : public reflection::description::PropertyInfoImpl
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::MethodInfoImpl				MethodInfoImpl;
				typedef reflection::description::EventInfoImpl				EventInfoImpl;
			public:
				WfProperty(ITypeDescriptor* ownerTypeDescriptor, const WString& name);
				~WfProperty();

				void									SetGetter(MethodInfoImpl* value);
				void									SetSetter(MethodInfoImpl* value);
				void									SetValueChangedEvent(EventInfoImpl* value);
			};

/***********************************************************************
Custom Type
***********************************************************************/

			class WfCustomType : public reflection::description::TypeDescriptorImpl
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::ITypeInfo					ITypeInfo;
			protected:
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;
				
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
				void									LoadInternal()override;
			public:
				WfCustomType(const WString& typeName);
				~WfCustomType();
				
				runtime::WfRuntimeGlobalContext*		GetGlobalContext();
				void									AddBaseType(ITypeDescriptor* type);
				void									AddMember(const WString& name, Ptr<WfMethodBase> value);
				void									AddMember(Ptr<WfProperty> value);
				void									AddMember(Ptr<WfEvent> value);
			};

			class WfClass : public WfCustomType
			{
				friend class WfTypeImpl;
			public:
				WfClass(const WString& typeName);
				~WfClass();
			};

			class WfInterface : public WfCustomType
			{
				friend class WfTypeImpl;
			public:
				WfInterface(const WString& typeName);
				~WfInterface();
			};

/***********************************************************************
Plugin
***********************************************************************/

			class WfTypeImpl : public Object, public reflection::description::ITypeLoader, public reflection::Description<WfTypeImpl>
			{
			protected:
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;

			public:
				collections::List<Ptr<WfClass>>			classes;
				collections::List<Ptr<WfInterface>>		interfaces;
				
				runtime::WfRuntimeGlobalContext*		GetGlobalContext();
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
				void									Load(reflection::description::ITypeManager* manager)override;
				void									Unload(reflection::description::ITypeManager* manager)override;
			};
		}
	}
}

#endif