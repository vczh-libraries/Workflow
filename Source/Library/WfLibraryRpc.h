/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::RPC

Interfaces:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_RPC
#define VCZH_WORKFLOW_LIBRARY_RPC

#include "WfLibraryPredefined.h"

namespace vl
{
	namespace rpc_controller
	{
		struct RpcObjectReference
		{
			vl::vint						clientId = 0;
			vl::vint						objectId = 0;
			vl::vint						typeId = 0;

			auto operator<=>(const RpcObjectReference&) const = default;
		};

		inline constexpr vint			RpcTypeId_IValueEnumerable = -1;
		inline constexpr vint			RpcTypeId_IValueEnumerator = -2;
		inline constexpr vint			RpcTypeId_IValueArray = -3;
		inline constexpr vint			RpcTypeId_IValueList = -4;
		inline constexpr vint			RpcTypeId_IValueObservableList = -5;
		inline constexpr vint			RpcTypeId_IValueDictionary = -6;

		class IRpcIdSync
			: public virtual vl::reflection::IDescriptable
			, public vl::reflection::Description<IRpcIdSync>
		{
		public:
			virtual void					SyncIds(vl::Ptr<vl::reflection::description::IValueDictionary> ids) = 0;
		};

		class IRpcListOps
			: public virtual vl::reflection::IDescriptable
			, public vl::reflection::Description<IRpcListOps>
		{
		public:
			virtual RpcObjectReference		EnumCreate(RpcObjectReference ref) = 0;
			virtual bool					EnumNext(RpcObjectReference enumerator) = 0;
			virtual vl::reflection::description::Value
										EnumGetCurrent(RpcObjectReference enumerator) = 0;

			virtual vl::vint				ListGetCount(RpcObjectReference ref) = 0;
			virtual vl::reflection::description::Value
										ListGet(RpcObjectReference ref, vl::vint index) = 0;
			virtual void					ListSet(RpcObjectReference ref, vl::vint index, const vl::reflection::description::Value& value) = 0;
			virtual vl::vint				ListAdd(RpcObjectReference ref, const vl::reflection::description::Value& value) = 0;
			virtual vl::vint				ListInsert(RpcObjectReference ref, vl::vint index, const vl::reflection::description::Value& value) = 0;
			virtual bool					ListRemoveAt(RpcObjectReference ref, vl::vint index) = 0;
			virtual void					ListClear(RpcObjectReference ref) = 0;
			virtual bool					ListContains(RpcObjectReference ref, const vl::reflection::description::Value& value) = 0;
			virtual vl::vint				ListIndexOf(RpcObjectReference ref, const vl::reflection::description::Value& value) = 0;

			virtual vl::vint				DictGetCount(RpcObjectReference ref) = 0;
			virtual vl::reflection::description::Value
										DictGet(RpcObjectReference ref, const vl::reflection::description::Value& key) = 0;
			virtual void					DictSet(RpcObjectReference ref, const vl::reflection::description::Value& key, const vl::reflection::description::Value& value) = 0;
			virtual bool					DictRemove(RpcObjectReference ref, const vl::reflection::description::Value& key) = 0;
			virtual void					DictClear(RpcObjectReference ref) = 0;
			virtual bool					DictContainsKey(RpcObjectReference ref, const vl::reflection::description::Value& key) = 0;
			virtual vl::Ptr<vl::reflection::description::IValueArray>
										DictGetKeys(RpcObjectReference ref) = 0;
			virtual vl::Ptr<vl::reflection::description::IValueArray>
										DictGetValues(RpcObjectReference ref) = 0;
		};

		class IRpcObjectOps
			: public virtual IRpcIdSync
			, public vl::reflection::Description<IRpcObjectOps>
		{
		public:
			virtual vl::reflection::description::Value
										InvokeMethod(
											RpcObjectReference ref,
											vl::vint methodId,
											vl::Ptr<vl::reflection::description::IValueArray> arguments
										) = 0;

			virtual vl::Ptr<vl::reflection::description::IAsync>
										InvokeMethodAsync(
											RpcObjectReference ref,
											vl::vint methodId,
											vl::Ptr<vl::reflection::description::IValueArray> arguments
										) = 0;

			virtual void					ObjectHold(RpcObjectReference ref, bool hold) = 0;
			virtual RpcObjectReference		RequestService(vl::vint typeId) = 0;
		};

		class IRpcListEventOps
			: public virtual vl::reflection::IDescriptable
			, public vl::reflection::Description<IRpcListEventOps>
		{
		public:
			virtual void					OnItemChanged(RpcObjectReference ref, vl::vint index, vl::vint oldCount, vl::vint newCount) = 0;
		};

		class IRpcObjectEventOps
			: public virtual IRpcIdSync
			, public vl::reflection::Description<IRpcObjectEventOps>
		{
		public:
			virtual void					InvokeEvent(
											RpcObjectReference ref,
											vl::vint eventId,
											vl::Ptr<vl::reflection::description::IValueArray> arguments
										) = 0;
		};

		class IRpcController
			: public virtual IRpcListOps
			, public virtual IRpcObjectOps
			, public virtual IRpcListEventOps
			, public virtual IRpcObjectEventOps
			, public vl::reflection::Description<IRpcController>
		{
		public:
			virtual vl::Ptr<vl::reflection::description::IValueDictionary>
										Register(
											vl::Ptr<IRpcObjectOps> objectCallback,
											vl::Ptr<IRpcObjectEventOps> eventCallback,
											vl::Ptr<IRpcListOps> listCallback,
											vl::Ptr<IRpcListEventOps> listEventCallback
										) = 0;

			virtual RpcObjectReference		RegisterLocalObject(vl::vint typeId) = 0;
			virtual void					UnregisterLocalObject(RpcObjectReference ref) = 0;

			virtual void					AcquireRemoteObject(RpcObjectReference ref) = 0;
			virtual void					ReleaseRemoteObject(RpcObjectReference ref) = 0;
		};

		class IRpcLifeCycle
			: public virtual vl::reflection::IDescriptable
			, public vl::reflection::Description<IRpcLifeCycle>
		{
		public:
			virtual vl::Ptr<IRpcController>	GetController()const = 0;
			virtual vl::Ptr<vl::reflection::IDescriptable>
										RefToPtr(RpcObjectReference ref) = 0;
			virtual RpcObjectReference		PtrToRef(vl::Ptr<vl::reflection::IDescriptable> obj) = 0;
		};

		class RpcByrefListEventDispatcher : public Object, public IRpcListEventOps
		{
		private:
			vl::collections::Dictionary<vint, vl::Ptr<vl::reflection::description::IValueObservableList>>
										targets;
		public:
			void							Track(vint objectId, vl::Ptr<vl::reflection::description::IValueObservableList> list);
			void							Untrack(vint objectId);
			void							OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};

		class RpcByrefEnumerator : public Object, public vl::reflection::description::IValueEnumerator
		{
		private:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			RpcObjectReference				ref;
			vint							index = -1;
		public:
			RpcByrefEnumerator(IRpcLifeCycle* lc, RpcObjectReference enumeratorRef);
			~RpcByrefEnumerator();

			vl::reflection::description::Value
										GetCurrent()override;
			vint							GetIndex()override;
			bool							Next()override;
		};

		class RpcByrefEnumerable : public Object, public vl::reflection::description::IValueEnumerable
		{
		private:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			RpcObjectReference				ref;
		public:
			RpcByrefEnumerable(IRpcLifeCycle* lc, RpcObjectReference enumerableRef);
			~RpcByrefEnumerable();

			vl::Ptr<vl::reflection::description::IValueEnumerator>
										CreateEnumerator()override;
		};

		class RpcByrefList : public Object, public vl::reflection::description::IValueList
		{
		protected:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			RpcObjectReference				ref;
		public:
			RpcByrefList(IRpcLifeCycle* lc, RpcObjectReference listRef);
			~RpcByrefList()override;

			vl::Ptr<vl::reflection::description::IValueEnumerator>
										CreateEnumerator()override;
			vint							GetCount()override;
			vl::reflection::description::Value
										Get(vint index)override;
			bool							Contains(const vl::reflection::description::Value& value)override;
			vint							IndexOf(const vl::reflection::description::Value& value)override;
			void							Set(vint index, const vl::reflection::description::Value& value)override;
			vint							Add(const vl::reflection::description::Value& value)override;
			vint							Insert(vint index, const vl::reflection::description::Value& value)override;
			bool							Remove(const vl::reflection::description::Value& value)override;
			bool							RemoveAt(vint index)override;
			void							Clear()override;
		};

		class RpcByrefArray : public Object, public vl::reflection::description::IValueArray
		{
		private:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			RpcObjectReference				ref;
		public:
			RpcByrefArray(IRpcLifeCycle* lc, RpcObjectReference arrayRef);
			~RpcByrefArray();

			vl::Ptr<vl::reflection::description::IValueEnumerator>
										CreateEnumerator()override;
			vint							GetCount()override;
			vl::reflection::description::Value
										Get(vint index)override;
			bool							Contains(const vl::reflection::description::Value& value)override;
			vint							IndexOf(const vl::reflection::description::Value& value)override;
			void							Set(vint index, const vl::reflection::description::Value& value)override;
			void							Resize(vint size)override;
		};

		class RpcByrefObservableList : public Object, public vl::reflection::description::IValueObservableList
		{
		private:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			RpcObjectReference				ref;
			vl::Ptr<RpcByrefListEventDispatcher>
										dispatcher;
		public:
			RpcByrefObservableList(IRpcLifeCycle* lc, RpcObjectReference listRef, vl::Ptr<RpcByrefListEventDispatcher> listDispatcher);
			~RpcByrefObservableList();

			vl::Ptr<vl::reflection::description::IValueEnumerator>
										CreateEnumerator()override;
			vint							GetCount()override;
			vl::reflection::description::Value
										Get(vint index)override;
			bool							Contains(const vl::reflection::description::Value& value)override;
			vint							IndexOf(const vl::reflection::description::Value& value)override;
			void							Set(vint index, const vl::reflection::description::Value& value)override;
			vint							Add(const vl::reflection::description::Value& value)override;
			vint							Insert(vint index, const vl::reflection::description::Value& value)override;
			bool							Remove(const vl::reflection::description::Value& value)override;
			bool							RemoveAt(vint index)override;
			void							Clear()override;
		};

		class RpcByrefDictionary : public Object, public vl::reflection::description::IValueDictionary
		{
		private:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			RpcObjectReference				ref;
		public:
			RpcByrefDictionary(IRpcLifeCycle* lc, RpcObjectReference dictRef);
			~RpcByrefDictionary();

			vl::Ptr<vl::reflection::description::IValueReadonlyList>
										GetKeys()override;
			vl::Ptr<vl::reflection::description::IValueReadonlyList>
										GetValues()override;
			vint							GetCount()override;
			vl::reflection::description::Value
										Get(const vl::reflection::description::Value& key)override;
			void							Set(const vl::reflection::description::Value& key, const vl::reflection::description::Value& value)override;
			bool							Remove(const vl::reflection::description::Value& key)override;
			void							Clear()override;
		};

		class RpcCalleeListOps : public Object, public IRpcListOps
		{
		private:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			vl::collections::Dictionary<vint, vl::Ptr<vl::reflection::description::IValueEnumerator>>
										enumerators;
			vl::collections::Dictionary<vint, vl::Ptr<vl::reflection::description::IValueArray>>
										arrays;
			vl::collections::Dictionary<vint, vl::Ptr<vl::reflection::description::IValueList>>
										lists;
			vl::collections::Dictionary<vint, vl::Ptr<vl::reflection::description::IValueDictionary>>
										dicts;
			vl::collections::Dictionary<vint, vl::Ptr<vl::reflection::description::IValueObservableList>>
										observableLists;
		public:
			RpcCalleeListOps(IRpcLifeCycle* lc);

			void							RegisterLocalObject(RpcObjectReference ref, vl::Ptr<vl::reflection::IDescriptable> obj);
			void							UnregisterLocalObject(RpcObjectReference ref);
			bool							HasTrackedObject(vint objectId)const;
			bool							HasTrackedEnumerator(vint objectId)const;

			RpcObjectReference				EnumCreate(RpcObjectReference ref)override;
			bool							EnumNext(RpcObjectReference enumerator)override;
			vl::reflection::description::Value
										EnumGetCurrent(RpcObjectReference enumerator)override;

			vint							ListGetCount(RpcObjectReference ref)override;
			vl::reflection::description::Value
										ListGet(RpcObjectReference ref, vint index)override;
			void							ListSet(RpcObjectReference ref, vint index, const vl::reflection::description::Value& value)override;
			vint							ListAdd(RpcObjectReference ref, const vl::reflection::description::Value& value)override;
			vint							ListInsert(RpcObjectReference ref, vint index, const vl::reflection::description::Value& value)override;
			bool							ListRemoveAt(RpcObjectReference ref, vint index)override;
			void							ListClear(RpcObjectReference ref)override;
			bool							ListContains(RpcObjectReference ref, const vl::reflection::description::Value& value)override;
			vint							ListIndexOf(RpcObjectReference ref, const vl::reflection::description::Value& value)override;

			vint							DictGetCount(RpcObjectReference ref)override;
			vl::reflection::description::Value
										DictGet(RpcObjectReference ref, const vl::reflection::description::Value& key)override;
			void							DictSet(RpcObjectReference ref, const vl::reflection::description::Value& key, const vl::reflection::description::Value& value)override;
			bool							DictRemove(RpcObjectReference ref, const vl::reflection::description::Value& key)override;
			void							DictClear(RpcObjectReference ref)override;
			bool							DictContainsKey(RpcObjectReference ref, const vl::reflection::description::Value& key)override;
			vl::Ptr<vl::reflection::description::IValueArray>
										DictGetKeys(RpcObjectReference ref)override;
			vl::Ptr<vl::reflection::description::IValueArray>
										DictGetValues(RpcObjectReference ref)override;
		};

		class RpcCalleeListEventBridge : public Object, public IRpcListEventOps
		{
		private:
			IRpcLifeCycle*					lifeCycle = nullptr;
			IRpcController*					controller = nullptr;
			vl::collections::Dictionary<vint, RpcObjectReference>
										refs;
			vl::collections::Dictionary<vint, vl::Ptr<vl::reflection::description::IValueObservableList>>
										sources;
			vl::collections::Dictionary<vint, vl::Ptr<vl::EventHandler>>
										handlers;
		public:
			RpcCalleeListEventBridge(IRpcLifeCycle* lc);

			void							RegisterLocalObject(RpcObjectReference ref, vl::Ptr<vl::reflection::IDescriptable> obj);
			void							UnregisterLocalObject(RpcObjectReference ref);
			bool							HasTrackedHandler(vint objectId)const;

			void							OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};

		vl::reflection::description::Value RpcBoxByref(const vl::reflection::description::Value& trivial, IRpcLifeCycle* lc);
		vl::reflection::description::Value RpcUnboxByref(const vl::reflection::description::Value& serializable, IRpcLifeCycle* lc);
		vl::reflection::description::Value RpcBoxByval(const vl::reflection::description::Value& trivial, IRpcLifeCycle* lc);
		vl::reflection::description::Value RpcUnboxByval(const vl::reflection::description::Value& serializable, IRpcLifeCycle* lc);
	}
}

#endif
