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
		constexpr vint RpcTypeId_NotFound = -100;
		constexpr vint RpcClientId_Invalid = -1;
		constexpr vint RpcObjectId_Invalid = -1;

		struct RpcObjectReference
		{
			vint							clientId = RpcClientId_Invalid;
			vint							objectId = RpcObjectId_Invalid;
			vint							typeId = RpcTypeId_NotFound;

			auto operator<=>(const RpcObjectReference&) const = default;
		};

		inline constexpr vint				RpcTypeId_IValueEnumerable = -1;
		inline constexpr vint				RpcTypeId_IValueEnumerator = -2;
		inline constexpr vint				RpcTypeId_IValueArray = -3;
		inline constexpr vint				RpcTypeId_IValueList = -4;
		inline constexpr vint				RpcTypeId_IValueObservableList = -5;
		inline constexpr vint				RpcTypeId_IValueDictionary = -6;
		inline constexpr vint				RpcTypeId_IValueReadonlyList = -7;
		
/***********************************************************************
* Interfaces (Operations)
***********************************************************************/

		class IRpcListOps
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcListOps>
		{
		public:
			virtual RpcObjectReference								EnumCreate(RpcObjectReference ref) = 0;
			virtual bool											EnumNext(RpcObjectReference enumerator) = 0;
			virtual reflection::description::Value					EnumGetCurrent(RpcObjectReference enumerator) = 0;

			virtual vint											ListGetCount(RpcObjectReference ref) = 0;
			virtual reflection::description::Value					ListGet(RpcObjectReference ref, vint index) = 0;
			virtual void											ListSet(RpcObjectReference ref, vint index, const reflection::description::Value& value) = 0;
			virtual vint											ListAdd(RpcObjectReference ref, const reflection::description::Value& value) = 0;
			virtual vint											ListInsert(RpcObjectReference ref, vint index, const reflection::description::Value& value) = 0;
			virtual bool											ListRemoveAt(RpcObjectReference ref, vint index) = 0;
			virtual void											ListClear(RpcObjectReference ref) = 0;
			virtual bool											ListContains(RpcObjectReference ref, const reflection::description::Value& value) = 0;
			virtual vint											ListIndexOf(RpcObjectReference ref, const reflection::description::Value& value) = 0;

			virtual vint											DictGetCount(RpcObjectReference ref) = 0;
			virtual reflection::description::Value					DictGet(RpcObjectReference ref, const reflection::description::Value& key) = 0;
			virtual void											DictSet(RpcObjectReference ref, const reflection::description::Value& key, const reflection::description::Value& value) = 0;
			virtual bool											DictRemove(RpcObjectReference ref, const reflection::description::Value& key) = 0;
			virtual void											DictClear(RpcObjectReference ref) = 0;
			virtual bool											DictContainsKey(RpcObjectReference ref, const reflection::description::Value& key) = 0;
			virtual RpcObjectReference								DictGetKeys(RpcObjectReference ref) = 0;
			virtual RpcObjectReference								DictGetValues(RpcObjectReference ref) = 0;
		};

		class IRpcObjectOps
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcObjectOps>
		{
		public:
			virtual reflection::description::Value					InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments) = 0;
			virtual void											ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold) = 0;
			virtual void											RegisterService(vint typeId, Ptr<reflection::IDescriptable> service) = 0;
		};

		class IRpcListEventOps
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcListEventOps>
		{
		public:
			virtual void											OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount) = 0;
		};

		class IRpcObjectEventOps
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcObjectEventOps>
		{
		public:
			virtual void											InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments) = 0;
		};
		
/***********************************************************************
* Interfaces (Lifecycle)
***********************************************************************/

		class IRpcOperations
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcOperations>
		{
		public:
			virtual IRpcListOps*					GetListOps() = 0;
			virtual IRpcObjectOps*					GetObjectOps() = 0;
			virtual IRpcListEventOps*				GetListEventOps() = 0;
			virtual IRpcObjectEventOps*				GetObjectEventOps() = 0;
		};

		class IRpcDispatcher
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcDispatcher>
		{
		public:
			virtual void							Finalize() = 0;
			virtual bool							IsRegisteredService(RpcObjectReference ref) = 0;
			virtual void							RegisterService(vint typeId, RpcObjectReference ref) = 0;
			virtual RpcObjectReference				RequestService(vint typeId) = 0;

			virtual IRpcListEventOps*				BroadcastFromClient_ListEventOps(vint selfClientId) = 0;
			virtual IRpcObjectEventOps*				BroadcastFromClient_ObjectEventOps(vint selfClientId) = 0;
			virtual IRpcListOps*					SendToClient_ListOps(vint targetClientId) = 0;
			virtual IRpcObjectOps*					SendToClient_ObjectOps(vint targetClientId) = 0;
		};

		class IRpcController
			: public virtual IRpcOperations
			, public reflection::Description<IRpcController>
		{
		public:
			virtual void							Finalize() = 0;
			virtual void							SetEventSuppressedFlag(RpcObjectReference ref, vint eventId, bool suppressed) = 0;
			virtual bool							GetEventSuppressedFlag(RpcObjectReference ref, vint eventId) = 0;
			virtual void							SetItemChangedSuppressedFlag(RpcObjectReference ref, bool suppressed) = 0;
			virtual bool							GetItemChangedSuppressedFlag(RpcObjectReference ref) = 0;
		};

		class IRpcLifecycle
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcLifecycle>
		{
		public:
			virtual void							Finalize() = 0;
			virtual vint							GetClientId() = 0;
			virtual IRpcDispatcher*					GetDispatcher() = 0;
			virtual IRpcController*					GetController() = 0;
			virtual Ptr<reflection::IDescriptable>	RefToPtr(RpcObjectReference ref) = 0;
			virtual RpcObjectReference				PtrToRef(Ptr<reflection::IDescriptable> obj) = 0;
			virtual void							LocalObjectHold(RpcObjectReference ref, vint remoteClientId) = 0;
			virtual void							LocalObjectUnhold(RpcObjectReference ref, vint remoteClientId) = 0;
			virtual void							RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service) = 0;
			virtual Ptr<reflection::IDescriptable>	RequestService(const WString& fullName) = 0;
		};

		class IRpcWrapperBase
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcWrapperBase>
		{
		public:
			virtual void							DisconnectFromLifecycle() = 0;
		};
		
/***********************************************************************
* Collection Caller Wrappers
**********************************************************************/

		class RpcByrefEnumerator : public Object, public reflection::Description<RpcByrefEnumerator>, public reflection::description::IValueEnumerator, public virtual IRpcWrapperBase
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;
			RpcObjectReference								ref;
			vint											index = -1;
		public:
			RpcByrefEnumerator(IRpcLifecycle* lc, RpcObjectReference enumeratorRef);
			~RpcByrefEnumerator();

			void											DisconnectFromLifecycle()override;
			reflection::description::Value					GetCurrent()override;
			vint											GetIndex()override;
			bool											Next()override;
		};

		class RpcByrefEnumerable : public Object, public reflection::Description<RpcByrefEnumerable>, public reflection::description::IValueEnumerable, public virtual IRpcWrapperBase
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefEnumerable(IRpcLifecycle* lc, RpcObjectReference enumerableRef);
			~RpcByrefEnumerable();

			void											DisconnectFromLifecycle()override;
			Ptr<reflection::description::IValueEnumerator>	CreateEnumerator()override;
		};

		class RpcByrefReadonlyList : public Object, public reflection::Description<RpcByrefReadonlyList>, public virtual reflection::description::IValueReadonlyList, public virtual IRpcWrapperBase
		{
		protected:
			IRpcLifecycle*									lifecycle = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefReadonlyList(IRpcLifecycle* lc, RpcObjectReference listRef);
			~RpcByrefReadonlyList();

			void											DisconnectFromLifecycle()override;
			Ptr<reflection::description::IValueEnumerator>	CreateEnumerator()override;
			vint											GetCount()override;
			reflection::description::Value					Get(vint index)override;
			bool											Contains(const reflection::description::Value& value)override;
			vint											IndexOf(const reflection::description::Value& value)override;
		};

		class RpcByrefList : public RpcByrefReadonlyList, public reflection::Description<RpcByrefList>, public virtual reflection::description::IValueList
		{
		public:
			RpcByrefList(IRpcLifecycle* lc, RpcObjectReference listRef);
			~RpcByrefList()override;

			Ptr<reflection::description::IValueEnumerator>	CreateEnumerator()override;
			vint											GetCount()override;
			reflection::description::Value					Get(vint index)override;
			bool											Contains(const reflection::description::Value& value)override;
			vint											IndexOf(const reflection::description::Value& value)override;
			void											Set(vint index, const reflection::description::Value& value)override;
			vint											Add(const reflection::description::Value& value)override;
			vint											Insert(vint index, const reflection::description::Value& value)override;
			bool											Remove(const reflection::description::Value& value)override;
			bool											RemoveAt(vint index)override;
			void											Clear()override;
		};

		class RpcByrefArray : public Object, public reflection::Description<RpcByrefArray>, public reflection::description::IValueArray, public virtual IRpcWrapperBase
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefArray(IRpcLifecycle* lc, RpcObjectReference arrayRef);
			~RpcByrefArray();

			void											DisconnectFromLifecycle()override;
			Ptr<reflection::description::IValueEnumerator>	CreateEnumerator()override;
			vint											GetCount()override;
			reflection::description::Value					Get(vint index)override;
			bool											Contains(const reflection::description::Value& value)override;
			vint											IndexOf(const reflection::description::Value& value)override;
			void											Set(vint index, const reflection::description::Value& value)override;
			void											Resize(vint size)override;
		};

		class RpcByrefObservableList : public Object, public reflection::Description<RpcByrefObservableList>, public reflection::description::IValueObservableList, public virtual IRpcWrapperBase
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefObservableList(IRpcLifecycle* lc, RpcObjectReference listRef);
			~RpcByrefObservableList();

			void											DisconnectFromLifecycle()override;
			Ptr<reflection::description::IValueEnumerator>	CreateEnumerator()override;
			vint											GetCount()override;
			reflection::description::Value					Get(vint index)override;
			bool											Contains(const reflection::description::Value& value)override;
			vint											IndexOf(const reflection::description::Value& value)override;
			void											Set(vint index, const reflection::description::Value& value)override;
			vint											Add(const reflection::description::Value& value)override;
			vint											Insert(vint index, const reflection::description::Value& value)override;
			bool											Remove(const reflection::description::Value& value)override;
			bool											RemoveAt(vint index)override;
			void											Clear()override;
		};

		class RpcByrefDictionary : public Object, public reflection::Description<RpcByrefDictionary>, public reflection::description::IValueDictionary, public virtual IRpcWrapperBase
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefDictionary(IRpcLifecycle* lc, RpcObjectReference dictRef);
			~RpcByrefDictionary();

			void												DisconnectFromLifecycle()override;
			Ptr<reflection::description::IValueReadonlyList>	GetKeys()override;
			Ptr<reflection::description::IValueReadonlyList>	GetValues()override;
			vint												GetCount()override;
			reflection::description::Value						Get(const reflection::description::Value& key)override;
			void												Set(const reflection::description::Value& key, const reflection::description::Value& value)override;
			bool												Remove(const reflection::description::Value& key)override;
			void												Clear()override;
		};
		
/***********************************************************************
* Collection Callee Wrappers
***********************************************************************/

		class RpcCalleeListOps : public Object, public IRpcListOps
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;

		public:
			RpcCalleeListOps(IRpcLifecycle* lc);

			RpcObjectReference								EnumCreate(RpcObjectReference ref)override;
			bool											EnumNext(RpcObjectReference enumerator)override;
			reflection::description::Value					EnumGetCurrent(RpcObjectReference enumerator)override;

			vint											ListGetCount(RpcObjectReference ref)override;
			reflection::description::Value					ListGet(RpcObjectReference ref, vint index)override;
			void											ListSet(RpcObjectReference ref, vint index, const reflection::description::Value& value)override;
			vint											ListAdd(RpcObjectReference ref, const reflection::description::Value& value)override;
			vint											ListInsert(RpcObjectReference ref, vint index, const reflection::description::Value& value)override;
			bool											ListRemoveAt(RpcObjectReference ref, vint index)override;
			void											ListClear(RpcObjectReference ref)override;
			bool											ListContains(RpcObjectReference ref, const reflection::description::Value& value)override;
			vint											ListIndexOf(RpcObjectReference ref, const reflection::description::Value& value)override;

			vint											DictGetCount(RpcObjectReference ref)override;
			reflection::description::Value					DictGet(RpcObjectReference ref, const reflection::description::Value& key)override;
			void											DictSet(RpcObjectReference ref, const reflection::description::Value& key, const reflection::description::Value& value)override;
			bool											DictRemove(RpcObjectReference ref, const reflection::description::Value& key)override;
			void											DictClear(RpcObjectReference ref)override;
			bool											DictContainsKey(RpcObjectReference ref, const reflection::description::Value& key)override;
			RpcObjectReference								DictGetKeys(RpcObjectReference ref)override;
			RpcObjectReference								DictGetValues(RpcObjectReference ref)override;
		};

		class RpcCalleeListEventBridge : public Object, public IRpcListEventOps
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;

		public:
			RpcCalleeListEventBridge(IRpcLifecycle* lc);

			void											OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};
		
/***********************************************************************
* Helpers
***********************************************************************/

		RpcObjectReference									RpcBoxByref		(Ptr<reflection::IDescriptable> trivial, IRpcLifecycle* lc);
		Ptr<reflection::IDescriptable>						RpcUnboxByref	(RpcObjectReference serializable, IRpcLifecycle* lc);
		reflection::description::Value						RpcBoxByval		(Ptr<reflection::IDescriptable> trivial, IRpcLifecycle* lc);
		Ptr<reflection::IDescriptable>						RpcUnboxByval	(const reflection::description::Value& serializable, IRpcLifecycle* lc);
	}
}

#endif
