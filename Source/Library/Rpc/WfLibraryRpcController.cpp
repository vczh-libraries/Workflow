#include "WfLibraryRpcController.h"

namespace vl
{
	namespace rpc_controller
	{
		using namespace collections;

/***********************************************************************
* RpcControllerDefault
***********************************************************************/

		RpcControllerDefault::RpcControllerDefault()
		{
		}

		RpcControllerDefault::~RpcControllerDefault()
		{
		}

		void RpcControllerDefault::Register(Ptr<IRpcObjectOps> _objectCallback, Ptr<IRpcObjectEventOps> _eventCallback)
		{
			objectCallback = _objectCallback;
			eventCallback = _eventCallback;
		}

		IRpcObjectOps* RpcControllerDefault::GetObjectOps()
		{
			return objectCallback.Obj();
		}

		IRpcObjectEventOps* RpcControllerDefault::GetObjectEventOps()
		{
			return eventCallback.Obj();
		}

		void RpcControllerDefault::Finalize()
		{
		}

		template<typename TKey>
		void RpcControllerDefault::SetSuppressedFlag(Dictionary<TKey, vint>& flags, const TKey& key, bool suppressed)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller::RpcControllerDefault::SetSuppressedFlag(collections::Dictionary<TKey, vint>&, const TKey&, bool)#"
			auto index = flags.Keys().IndexOf(key);
			if (suppressed)
			{
				if (index == -1)
				{
					flags.Add(key, 1);
				}
				else
				{
					flags.Set(key, flags.Values()[index] + 1);
				}
			}
			else
			{
				CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Suppression counter is not set.");
				auto count = flags.Values()[index] - 1;
				CHECK_ERROR(count >= 0, ERROR_MESSAGE_PREFIX L"Suppression counter is negative.");
				if (count == 0)
				{
					flags.Remove(key);
				}
				else
				{
					flags.Set(key, count);
				}
			}
#undef ERROR_MESSAGE_PREFIX
		}

		template<typename TKey>
		bool RpcControllerDefault::GetSuppressedFlag(const Dictionary<TKey, vint>& flags, const TKey& key)
		{
			auto index = flags.Keys().IndexOf(key);
			return index != -1 && flags.Values()[index] > 0;
		}

		void RpcControllerDefault::SetEventSuppressedFlag(RpcObjectReference ref, vint eventId, bool suppressed)
		{
			SetSuppressedFlag(eventSuppressedFlags, { ref,eventId }, suppressed);
		}

		bool RpcControllerDefault::GetEventSuppressedFlag(RpcObjectReference ref, vint eventId)
		{
			return GetSuppressedFlag(eventSuppressedFlags, { ref,eventId });
		}

		void RpcControllerDefault::SetItemChangedSuppressedFlag(RpcObjectReference ref, bool suppressed)
		{
			SetSuppressedFlag(itemChangedSuppressedFlags, ref, suppressed);
		}

		bool RpcControllerDefault::GetItemChangedSuppressedFlag(RpcObjectReference ref)
		{
			return GetSuppressedFlag(itemChangedSuppressedFlags, ref);
		}
	}
}
