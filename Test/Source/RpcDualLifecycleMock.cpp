#include "RpcDualLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace rpc_controller;
		using namespace collections;
		using namespace reflection::description;

/***********************************************************************
* RpcDualLifecycleMock
***********************************************************************/

		void RpcDualLifecycleMock::AttachLocalObjectEvents(RpcObjectReference ref, reflection::IDescriptable* obj)
		{
			(void)ref;
			(void)obj;
		}

		IRpcDispatcher* RpcDualLifecycleMock::GetDispatcher()
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualLifecycleMock::GetDispatcher()#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"No dispatcher registered.");
			return dispatcher;
#undef ERROR_MESSAGE_PREFIX
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
		}

		void RpcDualDispatcherMockBase::Initialize()
		{
		}

		void RpcDualDispatcherMockBase::DeclareLocalService(RpcObjectReference ref)
		{
			GetOtherLifecycle(ref.clientId)->DeclareRemoteService(ref);
		}

		IRpcObjectEventOps* RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
			return GetOtherLifecycle(selfClientId)->GetController()->GetObjectEventOps();
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
		{
		}
	}
}
