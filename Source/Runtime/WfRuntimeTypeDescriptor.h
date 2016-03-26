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
		namespace typeimpl
		{
			class WfClassInstance;
			class WfInterfaceInstance;
		}
	}

	namespace reflection
	{
		template<>
		class Description<workflow::typeimpl::WfClassInstance> : public virtual DescriptableObject
		{
		private:
			description::ITypeDescriptor*				associatedTypeDescriptor;

		public:
			Description(description::ITypeDescriptor* _associatedTypeDescriptor)
				:associatedTypeDescriptor(_associatedTypeDescriptor)
			{
				typeDescriptor = &associatedTypeDescriptor;
			}
		};

		template<>
		class Description<workflow::typeimpl::WfInterfaceInstance> : public virtual DescriptableObject
		{
		private:
			description::ITypeDescriptor*				associatedTypeDescriptor;

		public:
			Description(description::ITypeDescriptor* _associatedTypeDescriptor)
				:associatedTypeDescriptor(_associatedTypeDescriptor)
			{
				typeDescriptor = &associatedTypeDescriptor;
			}
		};
	}

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

			class WfMethodProxy : public Object, public virtual reflection::description::IValueFunctionProxy
			{
				typedef reflection::description::IMethodInfo				IMethodInfo;
				typedef reflection::description::IValueList					IValueList;
				typedef reflection::description::Value						Value;
			protected:
				Value									thisObject;
				IMethodInfo*							methodInfo;

			public:
				WfMethodProxy(const Value& _thisObject, IMethodInfo* _methodInfo);
				~WfMethodProxy();
				
				Value									Invoke(Ptr<IValueList> arguments)override;
			};

			class WfMethodBase : public reflection::description::MethodInfoImpl
			{
				friend class WfCustomType;
				typedef reflection::description::ITypeInfo					ITypeInfo;
			protected:
				typedef reflection::description::Value						Value;
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;
				
				Value									CreateFunctionProxyInternal(const Value& thisObject)override;
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
			public:
				vint									functionIndex = -1;

				WfStaticMethod();
			};

/***********************************************************************
Class Method
***********************************************************************/

			class WfClassConstructor : public WfMethodBase
			{
				typedef reflection::description::Value						Value;
				typedef reflection::description::ITypeInfo					ITypeInfo;
			protected:

				Value									InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override;
			public:
				vint									functionIndex = -1;

				WfClassConstructor(Ptr<ITypeInfo> type);

				void									InvokeBaseCtor(const Value& thisObject, collections::Array<Value>& arguments);
			};

			class WfClassMethod : public WfMethodBase
			{
				typedef reflection::description::Value						Value;
			protected:

				Value									InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override;
			public:
				vint									functionIndex = -1;

				WfClassMethod();
			};

/***********************************************************************
Interface Method
***********************************************************************/

			class WfInterfaceConstructor : public WfMethodBase
			{
				typedef reflection::description::Value						Value;
				typedef reflection::description::ITypeInfo					ITypeInfo;
			protected:

				Value									InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override;
			public:
				WfInterfaceConstructor(Ptr<ITypeInfo> type);
			};

			class WfInterfaceMethod : public WfMethodBase
			{
				typedef reflection::description::Value						Value;
			protected:

				Value									InvokeInternal(const Value& thisObject, collections::Array<Value>& arguments)override;
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
				typedef collections::Group<WfEvent*, IEventHandler*>		EventHandlerGroup;

				class EventRecord : public Object
				{
				public:
					EventHandlerGroup					handlers;
				};

				static const wchar_t*					EventRecordInternalPropertyName;
			protected:

				Ptr<EventRecord>						GetEventRecord(DescriptableObject* thisObject, bool createIfNotExist);
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
Field
***********************************************************************/

			class WfField : public reflection::description::FieldInfoImpl
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::ITypeInfo					ITypeInfo;
				typedef reflection::description::Value						Value;
				typedef collections::Dictionary<WfField*, Value>			FieldValueMap;

				class FieldRecord : public Object
				{
				public:
					FieldValueMap						values;
				};

				static const wchar_t*					FieldRecordInternalPropertyName;
			protected:
				
				Ptr<FieldRecord>						GetFieldRecord(DescriptableObject* thisObject, bool createIfNotExist);
				Value									GetValueInternal(const Value& thisObject)override;
				void									SetValueInternal(Value& thisObject, const Value& newValue)override;
			public:
				WfField(ITypeDescriptor* ownerTypeDescriptor, const WString& name);
				~WfField();

				void									SetReturn(Ptr<ITypeInfo> typeInfo);
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
			protected:
				typedef reflection::description::TypeDescriptorFlags		TypeDescriptorFlags;
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::ITypeInfo					ITypeInfo;
				typedef reflection::description::IMethodGroupInfo			IMethodGroupInfo;
				typedef collections::List<ITypeDescriptor*>					TypeDescriptorList;
			protected:
				runtime::WfRuntimeGlobalContext*		globalContext = nullptr;
				bool									baseTypeExpanded = false;
				TypeDescriptorList						expandedBaseTypes;
				
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext, IMethodGroupInfo* group);
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
				void									LoadInternal()override;
			public:
				WfCustomType(TypeDescriptorFlags typeDescriptorFlags, const WString& typeName);
				~WfCustomType();
				
				runtime::WfRuntimeGlobalContext*		GetGlobalContext();
				const TypeDescriptorList&				GetExpandedBaseTypes();

				void									AddBaseType(ITypeDescriptor* type);
				void									AddMember(const WString& name, Ptr<WfMethodBase> value);
				void									AddMember(Ptr<WfClassConstructor> value);
				void									AddMember(Ptr<WfInterfaceConstructor> value);
				void									AddMember(Ptr<WfField> value);
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
Instance
***********************************************************************/

			class WfClassInstance : public Object, public reflection::Description<WfClassInstance>
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::Value						Value;
			protected:
				WfCustomType*							classType = nullptr;

			public:
				WfClassInstance(ITypeDescriptor* _typeDescriptor);
				~WfClassInstance();

				void									InstallBaseObject(ITypeDescriptor* td, Value& value);
			};

			class WfInterfaceInstance : public Object, public reflection::Description<WfInterfaceInstance>
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::IMethodInfo				IMethodInfo;
				typedef reflection::description::IValueInterfaceProxy		IValueInterfaceProxy;
			protected:
				Ptr<IValueInterfaceProxy>				proxy;

			public:
				WfInterfaceInstance(ITypeDescriptor* _typeDescriptor, Ptr<IValueInterfaceProxy> _proxy, collections::List<IMethodInfo*>& baseCtors);
				~WfInterfaceInstance();

				Ptr<IValueInterfaceProxy>				GetProxy();
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
