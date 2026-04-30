#include "RpcControllerMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace rpc_controller;
		using namespace collections;

/***********************************************************************
* RpcControllerMock
***********************************************************************/

		IRpcObjectOps* RpcControllerMock::RequireObjectCallback()const
		{
			CHECK_ERROR(objectCallback, L"RpcControllerMock requires an object callback.");
			return objectCallback.Obj();
		}

		IRpcObjectEventOps* RpcControllerMock::RequireEventCallback()const
		{
			CHECK_ERROR(eventCallback, L"RpcControllerMock requires an event callback.");
			return eventCallback.Obj();
		}

		IRpcListOps* RpcControllerMock::RequireListCallback()const
		{
			CHECK_ERROR(listCallback, L"RpcControllerMock requires a list callback.");
			return listCallback.Obj();
		}

		IRpcListEventOps* RpcControllerMock::RequireListEventCallback()const
		{
			CHECK_ERROR(listEventCallback, L"RpcControllerMock requires a list event callback.");
			return listEventCallback.Obj();
		}

		RpcControllerMock::RpcControllerMock()
		{
		}

		RpcControllerMock::~RpcControllerMock()
		{
		}

/***********************************************************************
* RpcControllerMock (IRpcController)
***********************************************************************/

		void RpcControllerMock::Register(Ptr<IRpcObjectOps> _objectCallback, Ptr<IRpcObjectEventOps> _eventCallback, Ptr<IRpcListOps> _listCallback, Ptr<IRpcListEventOps> _listEventCallback)
		{
			objectCallback = _objectCallback;
			eventCallback = _eventCallback;
			listCallback = _listCallback;
			listEventCallback = _listEventCallback;
		}

		IRpcListOps* RpcControllerMock::GetListOps()
		{
			return RequireListCallback();
		}

		IRpcObjectOps* RpcControllerMock::GetObjectOps()
		{
			return RequireObjectCallback();
		}

		IRpcListEventOps* RpcControllerMock::GetListEventOps()
		{
			return RequireListEventCallback();
		}

		IRpcObjectEventOps* RpcControllerMock::GetObjectEventOps()
		{
			return RequireEventCallback();
		}

		template<typename TKey>
		void SetSuppressedFlag(Dictionary<TKey, vint>& flags, const TKey& key, bool suppressed)
		{
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
				CHECK_ERROR(index != -1, L"RpcControllerMock::SetSuppressedFlag: Suppression counter is not set.");
				auto count = flags.Values()[index] - 1;
				CHECK_ERROR(count >= 0, L"RpcControllerMock::SetSuppressedFlag: Suppression counter is negative.");
				if (count == 0)
				{
					flags.Remove(key);
				}
				else
				{
					flags.Set(key, count);
				}
			}
		}

		template<typename TKey>
		bool GetSuppressedFlag(const Dictionary<TKey, vint>& flags, const TKey& key)
		{
			auto index = flags.Keys().IndexOf(key);
			return index != -1 && flags.Values()[index] > 0;
		}

		void RpcControllerMock::SetEventSuppressedFlag(RpcObjectReference ref, vint eventId, bool suppressed)
		{
			SetSuppressedFlag(eventSuppressedFlags, { ref,eventId }, suppressed);
		}

		bool RpcControllerMock::GetEventSuppressedFlag(RpcObjectReference ref, vint eventId)
		{
			return GetSuppressedFlag(eventSuppressedFlags, { ref,eventId });
		}

		void RpcControllerMock::SetItemChangedSuppressedFlag(RpcObjectReference ref, bool suppressed)
		{
			SetSuppressedFlag(itemChangedSuppressedFlags, ref, suppressed);
		}

		bool RpcControllerMock::GetItemChangedSuppressedFlag(RpcObjectReference ref)
		{
			return GetSuppressedFlag(itemChangedSuppressedFlags, ref);
		}
	}
}
