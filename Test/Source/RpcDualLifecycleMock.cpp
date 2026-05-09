#include "RpcDualLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace rpc_controller;
		using namespace collections;
		using namespace reflection::description;

		namespace
		{
			class RpcBroadcastObjectEventOpsMock : public Object, public IRpcObjectEventOps
			{
			private:
				List<IRpcObjectEventOps*> targets;

			public:
				void AddTarget(IRpcObjectEventOps* target)
				{
					targets.Add(target);
				}
				Value InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments)override
				{
					if (targets.Count() == 1)
					{
						return targets[0]->InvokeEvent(ref, eventId, arguments);
					}

					auto exceptions = CreateRpcEventExceptionMap();
					for (auto target : targets)
					{
						MergeRpcEventExceptionMap(exceptions, UnboxRpcEventExceptionMap(target->InvokeEvent(ref, eventId, arguments)));
					}
					return exceptions->GetCount() == 0 ? Value() : BoxRpcEventExceptionMap(exceptions);
				}
			};

			template<typename TInterface, typename TMock>
			TInterface* WrapOps(
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
					opsPtr = Ptr(new TMock(ops));
				}
				return opsPtr.Obj();
#undef ERROR_MESSAGE_PREFIX
			}

			class RpcListEventOpsMock : public Object, public IRpcListEventOps
			{
			private:
				IRpcListEventOps*							ops = nullptr;

			public:
				RpcListEventOpsMock(IRpcListEventOps* _ops)
					: ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcListEventOpsMock::RpcListEventOpsMock(...)#"
					CHECK_ERROR(ops, ERROR_MESSAGE_PREFIX L"Ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				Value OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)override
				{
					auto value = ops->OnItemChanged(ref, index, oldCount, newCount);
					ReadEventException(UnboxRpcEventExceptionMap(value));
					return value;
				}
			};

			class RpcObjectEventOpsMock : public Object, public IRpcObjectEventOps
			{
			private:
				IRpcObjectEventOps*							ops = nullptr;

			public:
				RpcObjectEventOpsMock(IRpcObjectEventOps* _ops)
					: ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcObjectEventOpsMock::RpcObjectEventOpsMock(...)#"
					CHECK_ERROR(ops, ERROR_MESSAGE_PREFIX L"Ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				Value InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments)override
				{
					auto value = ops->InvokeEvent(ref, eventId, arguments);
					ReadEventException(UnboxRpcEventExceptionMap(value));
					return value;
				}
			};

			class RpcListOpsMock : public Object, public IRpcListOps
			{
			private:
				IRpcListOps*								ops = nullptr;

			public:
				RpcListOpsMock(IRpcListOps* _ops)
					: ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcListOpsMock::RpcListOpsMock(...)#"
					CHECK_ERROR(ops, ERROR_MESSAGE_PREFIX L"Ops are required.");
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
					return ops->EnumGetCurrent(enumerator);
				}

				vint ListGetCount(RpcObjectReference ref)override
				{
					return ops->ListGetCount(ref);
				}

				Value ListGet(RpcObjectReference ref, vint index)override
				{
					return ops->ListGet(ref, index);
				}

				void ListSet(RpcObjectReference ref, vint index, const Value& value)override
				{
					ops->ListSet(ref, index, value);
				}

				vint ListAdd(RpcObjectReference ref, const Value& value)override
				{
					return ops->ListAdd(ref, value);
				}

				vint ListInsert(RpcObjectReference ref, vint index, const Value& value)override
				{
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
					return ops->ListContains(ref, value);
				}

				vint ListIndexOf(RpcObjectReference ref, const Value& value)override
				{
					return ops->ListIndexOf(ref, value);
				}

				vint DictGetCount(RpcObjectReference ref)override
				{
					return ops->DictGetCount(ref);
				}

				Value DictGet(RpcObjectReference ref, const Value& key)override
				{
					return ops->DictGet(ref, key);
				}

				void DictSet(RpcObjectReference ref, const Value& key, const Value& value)override
				{
					ops->DictSet(ref, key, value);
				}

				bool DictRemove(RpcObjectReference ref, const Value& key)override
				{
					return ops->DictRemove(ref, key);
				}

				void DictClear(RpcObjectReference ref)override
				{
					ops->DictClear(ref);
				}

				bool DictContainsKey(RpcObjectReference ref, const Value& key)override
				{
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

			class RpcObjectOpsMock : public Object, public IRpcObjectOps
			{
			private:
				IRpcObjectOps*								ops = nullptr;

			public:
				RpcObjectOpsMock(IRpcObjectOps* _ops)
					: ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcObjectOpsMock::RpcObjectOpsMock(...)#"
					CHECK_ERROR(ops, ERROR_MESSAGE_PREFIX L"Ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				Value InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override
				{
					auto value = ops->InvokeMethod(ref, methodId, arguments);
					ReadMethodException(value);
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
* RpcDualLifecycleMock
***********************************************************************/

		void RpcDualLifecycleMock::AttachLocalObjectEvents(RpcObjectReference ref, reflection::IDescriptable* obj)
		{
			(void)ref;
			(void)obj;
		}

		void RpcDualLifecycleMock::SetDispatcher(IRpcDispatcher* _dispatcher)
		{
			dispatcher = _dispatcher;
		}

/***********************************************************************
* RpcDualDispatcherMockBase
***********************************************************************/

		RpcDualLifecycleMock* RpcDualDispatcherMockBase::GetLifecycle(vint clientId)const
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMockBase::GetLifecycle(vint)const#"
			if (lifecycle1 && lifecycle1->GetClientId() == clientId) return lifecycle1;
			if (lifecycle2 && lifecycle2->GetClientId() == clientId) return lifecycle2;
			CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unknown client id.");
			return nullptr;
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualLifecycleMock* RpcDualDispatcherMockBase::GetOtherLifecycle(vint clientId)const
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMockBase::GetOtherLifecycle(vint)const#"
			if (lifecycle1 && lifecycle1->GetClientId() == clientId) return lifecycle2;
			if (lifecycle2 && lifecycle2->GetClientId() == clientId) return lifecycle1;
			CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unknown client id.");
			return nullptr;
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualDispatcherMockBase::RpcDualDispatcherMockBase(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2)
			: lifecycle1(lc1)
			, lifecycle2(lc2)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMockBase::RpcDualDispatcherMockBase(RpcDualLifecycleMock*, RpcDualLifecycleMock*)#"
			CHECK_ERROR(lifecycle1 && lifecycle2, ERROR_MESSAGE_PREFIX L"Two lifecycles are required.");
			lifecycle1->SetDispatcher(this);
			lifecycle2->SetDispatcher(this);
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualDispatcherMockBase::~RpcDualDispatcherMockBase()
		{
		}

		void RpcDualDispatcherMockBase::Finalize()
		{
			lifecycle2->Finalize();
			lifecycle1->Finalize();
			objectEventBroadcastOps2 = nullptr;
			objectEventBroadcastOps1 = nullptr;
			services.Clear();
		}

		bool RpcDualDispatcherMockBase::IsRegisteredService(RpcObjectReference ref)
		{
			return services.Values().Contains(ref);
		}

		void RpcDualDispatcherMockBase::RegisterService(vint typeId, RpcObjectReference ref)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMockBase::RegisterService(vint, RpcObjectReference)#"
			CHECK_ERROR(!services.Keys().Contains(typeId), ERROR_MESSAGE_PREFIX L"Service is already registered.");
			services.Set(typeId, ref);
			GetLifecycle(ref.clientId)->LocalObjectHold(ref, ref.clientId);
#undef ERROR_MESSAGE_PREFIX
		}

		RpcObjectReference RpcDualDispatcherMockBase::RequestService(vint typeId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMockBase::RequestService(vint)#"
			auto index = services.Keys().IndexOf(typeId);
			CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Service is not registered.");
			return services.Values()[index];
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcListEventOps* RpcDualDispatcherMockBase::BroadcastFromClient_ListEventOps(vint selfClientId)
		{
			return GetOtherLifecycle(selfClientId)->GetController()->GetListEventOps();
		}

		IRpcObjectEventOps* RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps(vint)#"
			Ptr<IRpcObjectEventOps>* broadcastOps = nullptr;
			if (lifecycle1 && lifecycle1->GetClientId() == selfClientId) broadcastOps = &objectEventBroadcastOps1;
			if (lifecycle2 && lifecycle2->GetClientId() == selfClientId) broadcastOps = &objectEventBroadcastOps2;
			CHECK_ERROR(broadcastOps, ERROR_MESSAGE_PREFIX L"Unknown client id.");
			if (!*broadcastOps)
			{
				auto ops = Ptr(new RpcBroadcastObjectEventOpsMock);
				ops->AddTarget(GetOtherLifecycle(selfClientId)->GetController()->GetObjectEventOps());
				*broadcastOps = ops;
			}
			return broadcastOps->Obj();
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcListOps* RpcDualDispatcherMockBase::SendToClient_ListOps(vint targetClientId)
		{
			return GetLifecycle(targetClientId)->GetController()->GetListOps();
		}

		IRpcObjectOps* RpcDualDispatcherMockBase::SendToClient_ObjectOps(vint targetClientId)
		{
			return GetLifecycle(targetClientId)->GetController()->GetObjectOps();
		}

/***********************************************************************
* RpcDualDispatcherMock
***********************************************************************/

		RpcDualDispatcherMock::RpcDualDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2)
			: RpcDualDispatcherMockBase(lc1, lc2)
			, lifecycle1(lc1)
			, lifecycle2(lc2)
		{
		}

		IRpcListEventOps* RpcDualDispatcherMock::BroadcastFromClient_ListEventOps(vint selfClientId)
		{
			auto ops = RpcDualDispatcherMockBase::BroadcastFromClient_ListEventOps(selfClientId);
			return WrapOps<IRpcListEventOps, RpcListEventOpsMock>(
				ops,
				lifecycle1->GetController()->GetListEventOps(),
				lifecycle2->GetController()->GetListEventOps(),
				listEventOps1,
				listEventOps2
				);
		}

		IRpcObjectEventOps* RpcDualDispatcherMock::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::BroadcastFromClient_ObjectEventOps(vint)#"
			auto ops = RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps(selfClientId);
			Ptr<IRpcObjectEventOps>* opsPtr = nullptr;
			if (lifecycle1 && lifecycle1->GetClientId() == selfClientId) opsPtr = &objectEventOps1;
			if (lifecycle2 && lifecycle2->GetClientId() == selfClientId) opsPtr = &objectEventOps2;
			CHECK_ERROR(opsPtr, ERROR_MESSAGE_PREFIX L"Unknown client id.");
			if (!*opsPtr)
			{
				*opsPtr = Ptr(new RpcObjectEventOpsMock(ops));
			}
			return opsPtr->Obj();
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcListOps* RpcDualDispatcherMock::SendToClient_ListOps(vint targetClientId)
		{
			auto ops = RpcDualDispatcherMockBase::SendToClient_ListOps(targetClientId);
			return WrapOps<IRpcListOps, RpcListOpsMock>(
				ops,
				lifecycle1->GetController()->GetListOps(),
				lifecycle2->GetController()->GetListOps(),
				listOps1,
				listOps2
				);
		}

		IRpcObjectOps* RpcDualDispatcherMock::SendToClient_ObjectOps(vint targetClientId)
		{
			auto ops = RpcDualDispatcherMockBase::SendToClient_ObjectOps(targetClientId);
			return WrapOps<IRpcObjectOps, RpcObjectOpsMock>(
				ops,
				lifecycle1->GetController()->GetObjectOps(),
				lifecycle2->GetController()->GetObjectOps(),
				objectOps1,
				objectOps2
				);
		}
	}
}
