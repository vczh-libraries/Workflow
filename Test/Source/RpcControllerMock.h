#ifndef VCZH_WORKFLOW_TEST_RPC_CONTROLLER_MOCK
#define VCZH_WORKFLOW_TEST_RPC_CONTROLLER_MOCK

#include "../../Source/Library/WfLibraryRpc.h"

namespace vl
{
	namespace rpc_controller_test
	{
		struct RpcEventSuppressionKey
		{
			rpc_controller::RpcObjectReference					ref;
			vint												eventId = 0;

			auto operator<=>(const RpcEventSuppressionKey&) const = default;
		};

		class RpcControllerMock : public Object, public rpc_controller::IRpcController
		{
		protected:
			Ptr<rpc_controller::IRpcObjectOps>									objectCallback;
			Ptr<rpc_controller::IRpcObjectEventOps>								eventCallback;
			Ptr<rpc_controller::IRpcListOps>									listCallback;
			Ptr<rpc_controller::IRpcListEventOps>								listEventCallback;
			collections::Dictionary<RpcEventSuppressionKey, vint>				eventSuppressedFlags;
			collections::Dictionary<rpc_controller::RpcObjectReference, vint>	itemChangedSuppressedFlags;

		public:

			RpcControllerMock();
			~RpcControllerMock();

			void												Register(Ptr<rpc_controller::IRpcObjectOps> objectCallback, Ptr<rpc_controller::IRpcObjectEventOps> eventCallback, Ptr<rpc_controller::IRpcListOps> listCallback, Ptr<rpc_controller::IRpcListEventOps> listEventCallback);

			// IRpcController

			rpc_controller::IRpcListOps*						GetListOps()override;
			rpc_controller::IRpcObjectOps*						GetObjectOps()override;
			rpc_controller::IRpcListEventOps*					GetListEventOps()override;
			rpc_controller::IRpcObjectEventOps*					GetObjectEventOps()override;

			void												SetEventSuppressedFlag(rpc_controller::RpcObjectReference ref, vint eventId, bool suppressed)override;
			bool												GetEventSuppressedFlag(rpc_controller::RpcObjectReference ref, vint eventId)override;
			void												SetItemChangedSuppressedFlag(rpc_controller::RpcObjectReference ref, bool suppressed)override;
			bool												GetItemChangedSuppressedFlag(rpc_controller::RpcObjectReference ref)override;
		};
	}
}

#endif
