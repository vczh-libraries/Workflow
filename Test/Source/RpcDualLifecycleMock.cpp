#include "RpcDualLifecycleMock.h"
#include "../../Source/Library/Rpc/WfLibraryRpcWrappers.h"

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
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::BroadcastFromClient_ListEventOps(vint)#"
			Ptr<IRpcListEventOps>* opsPtr = nullptr;
			IRpcSerializer* serializer = nullptr;
			if (lifecycle1 && lifecycle1->GetClientId() == selfClientId)
			{
				opsPtr = &listEventOps1;
				serializer = lifecycle1->GetSerializer();
			}
			if (lifecycle2 && lifecycle2->GetClientId() == selfClientId)
			{
				opsPtr = &listEventOps2;
				serializer = lifecycle2->GetSerializer();
			}
			CHECK_ERROR(opsPtr, ERROR_MESSAGE_PREFIX L"Unknown client id.");
			if (!*opsPtr)
			{
				*opsPtr = Ptr(new RpcCallerListEventOps(BroadcastFromClient_ObjectEventOps(selfClientId), serializer));
			}
			return opsPtr->Obj();
#undef ERROR_MESSAGE_PREFIX
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
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::SendToClient_ListOps(vint)#"
			Ptr<IRpcListOps>* opsPtr = nullptr;
			IRpcSerializer* serializer = nullptr;
			if (lifecycle1 && lifecycle1->GetClientId() == targetClientId)
			{
				opsPtr = &listOps1;
				serializer = lifecycle1->GetSerializer();
			}
			if (lifecycle2 && lifecycle2->GetClientId() == targetClientId)
			{
				opsPtr = &listOps2;
				serializer = lifecycle2->GetSerializer();
			}
			CHECK_ERROR(opsPtr, ERROR_MESSAGE_PREFIX L"Unknown client id.");
			if (!*opsPtr)
			{
				*opsPtr = Ptr(new RpcCallerListOps(SendToClient_ObjectOps(targetClientId), serializer));
			}
			return opsPtr->Obj();
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcObjectOps* RpcDualDispatcherMock::SendToClient_ObjectOps(vint targetClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::SendToClient_ObjectOps(vint)#"
			auto ops = RpcDualDispatcherMockBase::SendToClient_ObjectOps(targetClientId);
			Ptr<IRpcObjectOps>* opsPtr = nullptr;
			if (lifecycle1 && lifecycle1->GetClientId() == targetClientId) opsPtr = &objectOps1;
			if (lifecycle2 && lifecycle2->GetClientId() == targetClientId) opsPtr = &objectOps2;
			CHECK_ERROR(opsPtr, ERROR_MESSAGE_PREFIX L"Unknown client id.");
			if (!*opsPtr)
			{
				*opsPtr = Ptr(new RpcObjectOpsMock(ops));
			}
			return opsPtr->Obj();
#undef ERROR_MESSAGE_PREFIX
		}
	}
}
