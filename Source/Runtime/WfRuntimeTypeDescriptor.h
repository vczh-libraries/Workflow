/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIMETYPEDESCRIPTOR
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIMETYPEDESCRIPTOR

#include <Vlpp.h>

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
#define DEFINE_DESCRIPTION(TYPE)\
		template<>\
		class Description<workflow::typeimpl::TYPE> : public virtual DescriptableObject\
		{\
		private:\
			description::ITypeDescriptor*				associatedTypeDescriptor;\
		public:\
			Description(description::ITypeDescriptor* _associatedTypeDescriptor)\
				:associatedTypeDescriptor(_associatedTypeDescriptor)\
			{\
				typeDescriptor = &associatedTypeDescriptor;\
			}\
		};\

		DEFINE_DESCRIPTION(WfClassInstance)
		DEFINE_DESCRIPTION(WfInterfaceInstance)

#undef DEFINE_DESCRIPTION
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

				ICpp*									GetCpp()override;
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
				typedef reflection::description::IValueFunctionProxy		IValueFunctionProxy;
				typedef reflection::description::IValueList					IValueList;
				typedef reflection::description::Value						Value;

				class EventHandlerImpl : public Object, public IEventHandler
				{
				public:
					bool								isAttached = true;
					Ptr<IValueFunctionProxy>			proxy;

					EventHandlerImpl(Ptr<IValueFunctionProxy> _proxy)
						:proxy(_proxy)
					{
					}

					bool IsAttached()override
					{
						return isAttached;
					}
				};

				typedef collections::Group<WfEvent*, Ptr<EventHandlerImpl>>	EventHandlerGroup;
				class EventRecord : public Object
				{
				public:
					EventHandlerGroup					handlers;
				};

				static const wchar_t*					EventRecordInternalPropertyName;
			protected:

				Ptr<EventRecord>						GetEventRecord(DescriptableObject* thisObject, bool createIfNotExist);
				Ptr<IEventHandler>						AttachInternal(DescriptableObject* thisObject, Ptr<IValueFunctionProxy> handler)override;
				bool									DetachInternal(DescriptableObject* thisObject, Ptr<IEventHandler> handler)override;
				void									InvokeInternal(DescriptableObject* thisObject, Ptr<IValueList> arguments)override;
				Ptr<ITypeInfo>							GetHandlerTypeInternal()override;
			public:
				WfEvent(ITypeDescriptor* ownerTypeDescriptor, const WString& name);
				~WfEvent();

				ICpp*									GetCpp()override;
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

				ICpp*									GetCpp()override;
				void									SetReturn(Ptr<ITypeInfo> typeInfo);
			};

			class WfStructField : public reflection::description::FieldInfoImpl
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::ITypeInfo					ITypeInfo;
				typedef reflection::description::Value						Value;
			protected:

				Value									GetValueInternal(const Value& thisObject)override;
				void									SetValueInternal(Value& thisObject, const Value& newValue)override;
			public:
				WfStructField(ITypeDescriptor* ownerTypeDescriptor, const WString& name);
				~WfStructField();

				ICpp*									GetCpp()override;
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

			struct WfTypeInfoContent : reflection::description::TypeInfoContent
			{
				WString								workflowTypeName;

				WfTypeInfoContent(const WString& _workflowTypeName);
			};

			template<typename TBase>
			class WfCustomTypeBase : private WfTypeInfoContent, public TBase
			{
			public:
				WfCustomTypeBase(reflection::description::TypeDescriptorFlags typeDescriptorFlags, const WString& typeName)
					:WfTypeInfoContent(typeName)
					, TBase(typeDescriptorFlags, this)
				{
				}
			};

			class WfCustomType : public WfCustomTypeBase<reflection::description::TypeDescriptorImpl>
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
				vint									destructorFunctionIndex = -1;

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

			class WfStruct : public WfCustomTypeBase<reflection::description::ValueTypeDescriptorBase>
			{
				using FieldMap = collections::Dictionary<WString, Ptr<WfStructField>>;
				using IPropertyInfo = reflection::description::IPropertyInfo;
				using IValueType = reflection::description::IValueType;
				using IBoxedValue = reflection::description::IBoxedValue;

			protected:
				class WfValueType : public Object, public virtual IValueType
				{
					using Value = reflection::description::Value;

				protected:
					WfStruct*							owner;

				public:
					WfValueType(WfStruct* _owner);

					Value								CreateDefault()override;
					IBoxedValue::CompareResult			Compare(const Value& a, const Value& b)override;
				};

			protected:
				FieldMap								fields;

			public:
				WfStruct(const WString& typeName);
				~WfStruct();

				vint									GetPropertyCount()override;
				IPropertyInfo*							GetProperty(vint index)override;
				bool									IsPropertyExists(const WString& name, bool inheritable)override;
				IPropertyInfo*							GetPropertyByName(const WString& name, bool inheritable)override;

				void									AddMember(Ptr<WfStructField> value);
			};

			class WfEnum : public WfCustomTypeBase<reflection::description::ValueTypeDescriptorBase>
			{
				using EnumItemMap = collections::Dictionary<WString, vuint64_t>;
				using IValueType = reflection::description::IValueType;
				using IBoxedValue = reflection::description::IBoxedValue;
				using IEnumType = reflection::description::IEnumType;
				using Value = reflection::description::Value;

			protected:
				class WfValueType : public Object, public virtual IValueType
				{
				protected:
					WfEnum*							owner;

				public:
					WfValueType(WfEnum* _owner);

					Value								CreateDefault()override;
					IBoxedValue::CompareResult			Compare(const Value& a, const Value& b)override;
				};

				class WfEnumType : public Object, public virtual IEnumType
				{
				protected:
					WfEnum*								owner;

				public:
					WfEnumType(WfEnum* _owner);

					bool								IsFlagEnum()override;
					vint								GetItemCount()override;
					WString								GetItemName(vint index)override;
					vuint64_t							GetItemValue(vint index)override;
					vint								IndexOfItem(WString name)override;
					Value								ToEnum(vuint64_t value)override;
					vuint64_t							FromEnum(const Value& value)override;
				};

			protected:
				EnumItemMap								enumItems;

			public:
				WfEnum(bool isFlags, const WString& typeName);
				~WfEnum();

				void									AddEnumItem(const WString& name, vuint64_t value);
			};

/***********************************************************************
Instance
***********************************************************************/

			class WfClassInstance : public Object, public reflection::Description<WfClassInstance>
			{
				typedef reflection::description::ITypeDescriptor			ITypeDescriptor;
				typedef reflection::description::Value						Value;
			protected:
				WfClass*								classType = nullptr;

			public:
				WfClassInstance(ITypeDescriptor* _typeDescriptor);
				~WfClassInstance();

				void									InstallBaseObject(ITypeDescriptor* td, Value& value);
			};

			class WfInterfaceInstance : public Object, public reflection::IDescriptable, public reflection::Description<WfInterfaceInstance>
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

			struct WfStructInstance
			{
				using FieldValueMap = collections::Dictionary<WfStructField*, reflection::description::Value>;

				FieldValueMap							fieldValues;

				WfStructInstance()
				{
				}

				WfStructInstance(const WfStructInstance& si)
				{
					CopyFrom(fieldValues, si.fieldValues);
				}

				WfStructInstance& operator=(const WfStructInstance& si)
				{
					if (this != &si)
					{
						CopyFrom(fieldValues, si.fieldValues);
					}
					return *this;
				}
			};

			struct WfEnumInstance
			{
				vuint64_t								value = 0;
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
				collections::List<Ptr<WfStruct>>		structs;
				collections::List<Ptr<WfEnum>>			enums;
				
				runtime::WfRuntimeGlobalContext*		GetGlobalContext();
				void									SetGlobalContext(runtime::WfRuntimeGlobalContext* _globalContext);
				void									Load(reflection::description::ITypeManager* manager)override;
				void									Unload(reflection::description::ITypeManager* manager)override;
			};
		}
	}
}

#endif
