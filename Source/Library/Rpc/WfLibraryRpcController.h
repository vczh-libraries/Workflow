/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::RPC

Interfaces:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_RPC_CONTROLLER
#define VCZH_WORKFLOW_LIBRARY_RPC_CONTROLLER

#include "WfLibraryRpc.h"

namespace vl
{
	namespace rpc_controller
	{
		struct RpcEventSuppressionKey
		{
			RpcObjectReference					ref;
			vint								eventId = 0;

			auto operator<=>(const RpcEventSuppressionKey&) const = default;
		};

		class RpcControllerDefault : public Object, public IRpcController
		{
		protected:
			Ptr<IRpcObjectOps>										objectCallback;
			Ptr<IRpcObjectEventOps>									eventCallback;
			collections::Dictionary<RpcEventSuppressionKey, vint>	eventSuppressedFlags;
			collections::Dictionary<RpcObjectReference, vint>		itemChangedSuppressedFlags;

			template<typename TKey>
			static void												SetSuppressedFlag(collections::Dictionary<TKey, vint>& flags, const TKey& key, bool suppressed);

			template<typename TKey>
			static bool												GetSuppressedFlag(const collections::Dictionary<TKey, vint>& flags, const TKey& key);

		public:

			RpcControllerDefault();
			~RpcControllerDefault();

			void													Register(Ptr<IRpcObjectOps> objectCallback, Ptr<IRpcObjectEventOps> eventCallback);

			// IRpcController

			IRpcObjectOps*											GetObjectOps()override;
			IRpcObjectEventOps*										GetObjectEventOps()override;

			void													Finalize()override;
			void													SetEventSuppressedFlag(RpcObjectReference ref, vint eventId, bool suppressed)override;
			bool													GetEventSuppressedFlag(RpcObjectReference ref, vint eventId)override;
			void													SetItemChangedSuppressedFlag(RpcObjectReference ref, bool suppressed)override;
			bool													GetItemChangedSuppressedFlag(RpcObjectReference ref)override;
		};
	}
}

#endif
