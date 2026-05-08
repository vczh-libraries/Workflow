#include "RpcDualLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace rpc_controller;
		using namespace collections;

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

				RpcEventExceptionMap InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments)override
				{
					auto exceptions = CreateRpcEventExceptionMap();
					for (auto target : targets)
					{
						MergeRpcEventExceptionMap(exceptions, target->InvokeEvent(ref, eventId, arguments));
					}
					return exceptions->GetCount() == 0 ? nullptr : exceptions;
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
* RpcDualDispatcherMock
***********************************************************************/

		RpcDualLifecycleMock* RpcDualDispatcherMock::GetLifecycle(vint clientId)const
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::GetLifecycle(vint)const#"
			if (lifecycle1 && lifecycle1->GetClientId() == clientId) return lifecycle1;
			if (lifecycle2 && lifecycle2->GetClientId() == clientId) return lifecycle2;
			CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unknown client id.");
			return nullptr;
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualLifecycleMock* RpcDualDispatcherMock::GetOtherLifecycle(vint clientId)const
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::GetOtherLifecycle(vint)const#"
			if (lifecycle1 && lifecycle1->GetClientId() == clientId) return lifecycle2;
			if (lifecycle2 && lifecycle2->GetClientId() == clientId) return lifecycle1;
			CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unknown client id.");
			return nullptr;
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualDispatcherMock::RpcDualDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2)
			: lifecycle1(lc1)
			, lifecycle2(lc2)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::RpcDualDispatcherMock(RpcDualLifecycleMock*, RpcDualLifecycleMock*)#"
			CHECK_ERROR(lifecycle1 && lifecycle2, ERROR_MESSAGE_PREFIX L"Two lifecycles are required.");
			lifecycle1->SetDispatcher(this);
			lifecycle2->SetDispatcher(this);
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualDispatcherMock::~RpcDualDispatcherMock()
		{
		}

		void RpcDualDispatcherMock::Finalize()
		{
			lifecycle2->Finalize();
			lifecycle1->Finalize();
			objectEventBroadcastOps2 = nullptr;
			objectEventBroadcastOps1 = nullptr;
			services.Clear();
		}

		bool RpcDualDispatcherMock::IsRegisteredService(RpcObjectReference ref)
		{
			return services.Values().Contains(ref);
		}

		void RpcDualDispatcherMock::RegisterService(vint typeId, RpcObjectReference ref)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::RegisterService(vint, RpcObjectReference)#"
			CHECK_ERROR(!services.Keys().Contains(typeId), ERROR_MESSAGE_PREFIX L"Service is already registered.");
			services.Set(typeId, ref);
			GetLifecycle(ref.clientId)->LocalObjectHold(ref, ref.clientId);
#undef ERROR_MESSAGE_PREFIX
		}

		RpcObjectReference RpcDualDispatcherMock::RequestService(vint typeId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::RequestService(vint)#"
			auto index = services.Keys().IndexOf(typeId);
			CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Service is not registered.");
			return services.Values()[index];
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcListEventOps* RpcDualDispatcherMock::BroadcastFromClient_ListEventOps(vint selfClientId)
		{
			return GetOtherLifecycle(selfClientId)->GetController()->GetListEventOps();
		}

		IRpcObjectEventOps* RpcDualDispatcherMock::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualDispatcherMock::BroadcastFromClient_ObjectEventOps(vint)#"
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

		IRpcListOps* RpcDualDispatcherMock::SendToClient_ListOps(vint targetClientId)
		{
			return GetLifecycle(targetClientId)->GetController()->GetListOps();
		}

		IRpcObjectOps* RpcDualDispatcherMock::SendToClient_ObjectOps(vint targetClientId)
		{
			return GetLifecycle(targetClientId)->GetController()->GetObjectOps();
		}
	}
}
