/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::RPC

Collection Wrappers:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_RPC_WRAPPERS
#define VCZH_WORKFLOW_LIBRARY_RPC_WRAPPERS

#include "WfLibraryRpc.h"

namespace vl
{
	namespace rpc_controller
	{

/***********************************************************************
* Serialization
***********************************************************************/

		class IRpcSerializer
			: public virtual reflection::IDescriptable
			, public reflection::Description<IRpcSerializer>
		{
		public:
			virtual reflection::description::Value					Serialize(const reflection::description::Value& value) = 0;
			virtual reflection::description::Value					Deserialize(const reflection::description::Value& value) = 0;
		};

/***********************************************************************
* Collection Caller Wrappers
***********************************************************************/

		class RpcByrefEnumerator : public Object, public reflection::Description<RpcByrefEnumerator>, public reflection::description::IValueEnumerator, public virtual IRpcWrapperBase
		{
		private:
			IRpcLifecycle*									lifecycle = nullptr;
			IRpcSerializer*									serializer = nullptr;
			RpcObjectReference								ref;
			vint											index = -1;
		public:
			RpcByrefEnumerator(IRpcLifecycle* lc, RpcObjectReference enumeratorRef, IRpcSerializer* _serializer);
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
			IRpcSerializer*									serializer = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefEnumerable(IRpcLifecycle* lc, RpcObjectReference enumerableRef, IRpcSerializer* _serializer);
			~RpcByrefEnumerable();

			void											DisconnectFromLifecycle()override;
			Ptr<reflection::description::IValueEnumerator>	CreateEnumerator()override;
		};

		class RpcByrefReadonlyList : public Object, public reflection::Description<RpcByrefReadonlyList>, public virtual reflection::description::IValueReadonlyList, public virtual IRpcWrapperBase
		{
		protected:
			IRpcLifecycle*									lifecycle = nullptr;
			IRpcSerializer*									serializer = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefReadonlyList(IRpcLifecycle* lc, RpcObjectReference listRef, IRpcSerializer* _serializer);
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
			RpcByrefList(IRpcLifecycle* lc, RpcObjectReference listRef, IRpcSerializer* _serializer);
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
			IRpcSerializer*									serializer = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefArray(IRpcLifecycle* lc, RpcObjectReference arrayRef, IRpcSerializer* _serializer);
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
			IRpcSerializer*									serializer = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefObservableList(IRpcLifecycle* lc, RpcObjectReference listRef, IRpcSerializer* _serializer);
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
			IRpcSerializer*									serializer = nullptr;
			RpcObjectReference								ref;
		public:
			RpcByrefDictionary(IRpcLifecycle* lc, RpcObjectReference dictRef, IRpcSerializer* _serializer);
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
			IRpcSerializer*									serializer = nullptr;

		public:
			RpcCalleeListOps(IRpcLifecycle* lc, IRpcSerializer* _serializer);

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
			IRpcSerializer*									serializer = nullptr;

		public:
			RpcCalleeListEventBridge(IRpcLifecycle* lc, IRpcSerializer* _serializer);

			void											OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};
	}
}

#endif
