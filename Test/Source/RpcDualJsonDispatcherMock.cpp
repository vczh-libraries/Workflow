#include "RpcDualJsonDispatcherMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace collections;
		using namespace glr::json;
		using namespace reflection::description;
		using namespace rpc_controller;

		namespace
		{
			template<typename TInterface, typename TMock>
			TInterface* WrapOps(
				RpcDualJsonDispatcherMock* dispatcher,
				TInterface* ops,
				TInterface* ops1,
				TInterface* ops2,
				Ptr<TInterface>& opsPtr1,
				Ptr<TInterface>& opsPtr2)
			{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::WrapOps(...)#"
				CHECK_ERROR(ops == ops1 || ops == ops2, ERROR_MESSAGE_PREFIX L"Unexpected RPC ops.");
				auto& opsPtr = ops == ops1 ? opsPtr1 : opsPtr2;
				if (!opsPtr)
				{
					opsPtr = Ptr(new TMock(dispatcher, ops));
				}
				return opsPtr.Obj();
#undef ERROR_MESSAGE_PREFIX
			}

			class RpcJsonListEventOpsMock : public Object, public IRpcListEventOps
			{
			private:
				RpcDualJsonDispatcherMock*					dispatcher = nullptr;
				IRpcListEventOps*							ops = nullptr;

			public:
				RpcJsonListEventOpsMock(RpcDualJsonDispatcherMock* _dispatcher, IRpcListEventOps* _ops)
					: dispatcher(_dispatcher)
					, ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcJsonListEventOpsMock::RpcJsonListEventOpsMock(...)#"
					CHECK_ERROR(dispatcher && ops, ERROR_MESSAGE_PREFIX L"Dispatcher and ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				void OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override
				{
					ops->OnItemChanged(ref, index, oldCount, newCount);
				}
			};

			class RpcJsonObjectEventOpsMock : public Object, public IRpcObjectEventOps
			{
			private:
				RpcDualJsonDispatcherMock*					dispatcher = nullptr;
				IRpcObjectEventOps*							ops = nullptr;

			public:
				RpcJsonObjectEventOpsMock(RpcDualJsonDispatcherMock* _dispatcher, IRpcObjectEventOps* _ops)
					: dispatcher(_dispatcher)
					, ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcJsonObjectEventOpsMock::RpcJsonObjectEventOpsMock(...)#"
					CHECK_ERROR(dispatcher && ops, ERROR_MESSAGE_PREFIX L"Dispatcher and ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				void InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)override
				{
					dispatcher->RecordJsonArguments(arguments);
					ops->InvokeEvent(ref, eventId, arguments);
				}
			};

			class RpcJsonListOpsMock : public Object, public IRpcListOps
			{
			private:
				RpcDualJsonDispatcherMock*					dispatcher = nullptr;
				IRpcListOps*								ops = nullptr;

			public:
				RpcJsonListOpsMock(RpcDualJsonDispatcherMock* _dispatcher, IRpcListOps* _ops)
					: dispatcher(_dispatcher)
					, ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcJsonListOpsMock::RpcJsonListOpsMock(...)#"
					CHECK_ERROR(dispatcher && ops, ERROR_MESSAGE_PREFIX L"Dispatcher and ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				RpcObjectReference EnumCreate(RpcObjectReference ref)override
				{
					return ops->EnumCreate(ref);
				}

				bool EnumNext(RpcObjectReference enumerator)override
				{
					return ops->EnumNext(enumerator);
				}

				Value EnumGetCurrent(RpcObjectReference enumerator)override
				{
					auto value = ops->EnumGetCurrent(enumerator);
					dispatcher->RecordJsonValue(value);
					return value;
				}

				vint ListGetCount(RpcObjectReference ref)override
				{
					return ops->ListGetCount(ref);
				}

				Value ListGet(RpcObjectReference ref, vint index)override
				{
					auto value = ops->ListGet(ref, index);
					dispatcher->RecordJsonValue(value);
					return value;
				}

				void ListSet(RpcObjectReference ref, vint index, const Value& value)override
				{
					dispatcher->RecordJsonValue(value);
					ops->ListSet(ref, index, value);
				}

				vint ListAdd(RpcObjectReference ref, const Value& value)override
				{
					dispatcher->RecordJsonValue(value);
					return ops->ListAdd(ref, value);
				}

				vint ListInsert(RpcObjectReference ref, vint index, const Value& value)override
				{
					dispatcher->RecordJsonValue(value);
					return ops->ListInsert(ref, index, value);
				}

				bool ListRemoveAt(RpcObjectReference ref, vint index)override
				{
					return ops->ListRemoveAt(ref, index);
				}

				void ListClear(RpcObjectReference ref)override
				{
					ops->ListClear(ref);
				}

				bool ListContains(RpcObjectReference ref, const Value& value)override
				{
					dispatcher->RecordJsonValue(value);
					return ops->ListContains(ref, value);
				}

				vint ListIndexOf(RpcObjectReference ref, const Value& value)override
				{
					dispatcher->RecordJsonValue(value);
					return ops->ListIndexOf(ref, value);
				}

				vint DictGetCount(RpcObjectReference ref)override
				{
					return ops->DictGetCount(ref);
				}

				Value DictGet(RpcObjectReference ref, const Value& key)override
				{
					dispatcher->RecordJsonValue(key);
					auto value = ops->DictGet(ref, key);
					dispatcher->RecordJsonValue(value);
					return value;
				}

				void DictSet(RpcObjectReference ref, const Value& key, const Value& value)override
				{
					dispatcher->RecordJsonValue(key);
					dispatcher->RecordJsonValue(value);
					ops->DictSet(ref, key, value);
				}

				bool DictRemove(RpcObjectReference ref, const Value& key)override
				{
					dispatcher->RecordJsonValue(key);
					return ops->DictRemove(ref, key);
				}

				void DictClear(RpcObjectReference ref)override
				{
					ops->DictClear(ref);
				}

				bool DictContainsKey(RpcObjectReference ref, const Value& key)override
				{
					dispatcher->RecordJsonValue(key);
					return ops->DictContainsKey(ref, key);
				}

				RpcObjectReference DictGetKeys(RpcObjectReference ref)override
				{
					return ops->DictGetKeys(ref);
				}

				RpcObjectReference DictGetValues(RpcObjectReference ref)override
				{
					return ops->DictGetValues(ref);
				}
			};

			class RpcJsonObjectOpsMock : public Object, public IRpcObjectOps
			{
			private:
				RpcDualJsonDispatcherMock*					dispatcher = nullptr;
				IRpcObjectOps*								ops = nullptr;

			public:
				RpcJsonObjectOpsMock(RpcDualJsonDispatcherMock* _dispatcher, IRpcObjectOps* _ops)
					: dispatcher(_dispatcher)
					, ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcJsonObjectOpsMock::RpcJsonObjectOpsMock(...)#"
					CHECK_ERROR(dispatcher && ops, ERROR_MESSAGE_PREFIX L"Dispatcher and ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				Value InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)override
				{
					dispatcher->RecordJsonArguments(arguments);
					auto value = ops->InvokeMethod(ref, methodId, arguments);
					dispatcher->RecordJsonValue(value);
					return value;
				}

				void EndInvokeMethod(vint slot)override
				{
					ops->EndInvokeMethod(slot);
				}

				void ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold)override
				{
					ops->ObjectHold(ref, remoteClientId, hold);
				}

				void RegisterService(vint typeId, Ptr<reflection::IDescriptable> service)override
				{
					ops->RegisterService(typeId, service);
				}
			};
		}

/***********************************************************************
* RpcDualJsonDispatcherMock
***********************************************************************/

		RpcDualJsonDispatcherMock::RpcDualJsonDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2)
			: RpcDualDispatcherMock(lc1, lc2)
			, lifecycle1(lc1)
			, lifecycle2(lc2)
		{
		}

		void RpcDualJsonDispatcherMock::RecordJsonValue(const Value& value)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonDispatcherMock::RecordJsonValue(const Value&)#"
			Ptr<JsonNode> node;
			if (value.GetValueType() == Value::SharedPtr)
			{
				if (auto byvalReturnValue = value.GetSharedPtr().Cast<RpcByvalReturnValue>())
				{
					RecordJsonValue(byvalReturnValue->value);
					return;
				}
				node = value.GetSharedPtr().Cast<JsonNode>();
			}
			CHECK_ERROR(node, ERROR_MESSAGE_PREFIX L"RPC JSON value should be a Ptr<JsonNode>.");
			jsonValues.Add(node);
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcDualJsonDispatcherMock::RecordJsonArguments(Ptr<IValueArray> arguments)
		{
			for (vint i = 0; i < arguments->GetCount(); i++)
			{
				RecordJsonValue(arguments->Get(i));
			}
		}

		List<Ptr<JsonNode>>& RpcDualJsonDispatcherMock::JsonValues()
		{
			return jsonValues;
		}

		IRpcListEventOps* RpcDualJsonDispatcherMock::BroadcastFromClient_ListEventOps(vint selfClientId)
		{
			auto ops = RpcDualDispatcherMock::BroadcastFromClient_ListEventOps(selfClientId);
			return WrapOps<IRpcListEventOps, RpcJsonListEventOpsMock>(
				this,
				ops,
				lifecycle1->GetController()->GetListEventOps(),
				lifecycle2->GetController()->GetListEventOps(),
				listEventOps1,
				listEventOps2
				);
		}

		IRpcObjectEventOps* RpcDualJsonDispatcherMock::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
			auto ops = RpcDualDispatcherMock::BroadcastFromClient_ObjectEventOps(selfClientId);
			return WrapOps<IRpcObjectEventOps, RpcJsonObjectEventOpsMock>(
				this,
				ops,
				lifecycle1->GetController()->GetObjectEventOps(),
				lifecycle2->GetController()->GetObjectEventOps(),
				objectEventOps1,
				objectEventOps2
				);
		}

		IRpcListOps* RpcDualJsonDispatcherMock::SendToClient_ListOps(vint targetClientId)
		{
			auto ops = RpcDualDispatcherMock::SendToClient_ListOps(targetClientId);
			return WrapOps<IRpcListOps, RpcJsonListOpsMock>(
				this,
				ops,
				lifecycle1->GetController()->GetListOps(),
				lifecycle2->GetController()->GetListOps(),
				listOps1,
				listOps2
				);
		}

		IRpcObjectOps* RpcDualJsonDispatcherMock::SendToClient_ObjectOps(vint targetClientId)
		{
			auto ops = RpcDualDispatcherMock::SendToClient_ObjectOps(targetClientId);
			return WrapOps<IRpcObjectOps, RpcJsonObjectOpsMock>(
				this,
				ops,
				lifecycle1->GetController()->GetObjectOps(),
				lifecycle2->GetController()->GetObjectOps(),
				objectOps1,
				objectOps2
				);
		}
	}
}
