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

/***********************************************************************
* RpcControllerMock (IRpcObjectOps)
***********************************************************************/

		Value RpcControllerMock::InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
			return RequireObjectCallback()->InvokeMethod(ref, methodId, arguments);
		}

		Ptr<IAsync> RpcControllerMock::InvokeMethodAsync(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
			return RequireObjectCallback()->InvokeMethodAsync(ref, methodId, arguments);
		}

		void RpcControllerMock::ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold)
		{
			RequireObjectCallback()->ObjectHold(ref, remoteClientId, hold);
		}

		void RpcControllerMock::RegisterService(vint typeId, Ptr<IDescriptable> service)
		{
			RequireObjectCallback()->RegisterService(typeId, service);
		}

		Nullable<RpcObjectReference> RpcControllerMock::RequestService(vint typeId)
		{
			return RequireObjectCallback()->RequestService(typeId);
		}

/***********************************************************************
* RpcControllerMock (IRpcObjectEventOps)
***********************************************************************/

		void RpcControllerMock::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
			RequireEventCallback()->InvokeEvent(ref, eventId, arguments);
		}

/***********************************************************************
* RpcControllerMock (IRpcListOps)
***********************************************************************/

		RpcObjectReference RpcControllerMock::EnumCreate(RpcObjectReference ref)
		{
			return RequireListCallback()->EnumCreate(ref);
		}

		bool RpcControllerMock::EnumNext(RpcObjectReference enumerator)
		{
			return RequireListCallback()->EnumNext(enumerator);
		}

		Value RpcControllerMock::EnumGetCurrent(RpcObjectReference enumerator)
		{
			return RequireListCallback()->EnumGetCurrent(enumerator);
		}

		vint RpcControllerMock::ListGetCount(RpcObjectReference ref)
		{
			return RequireListCallback()->ListGetCount(ref);
		}

		Value RpcControllerMock::ListGet(RpcObjectReference ref, vint index)
		{
			return RequireListCallback()->ListGet(ref, index);
		}

		void RpcControllerMock::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			RequireListCallback()->ListSet(ref, index, value);
		}

		vint RpcControllerMock::ListAdd(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListAdd(ref, value);
		}

		vint RpcControllerMock::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			return RequireListCallback()->ListInsert(ref, index, value);
		}

		bool RpcControllerMock::ListRemoveAt(RpcObjectReference ref, vint index)
		{
			return RequireListCallback()->ListRemoveAt(ref, index);
		}

		void RpcControllerMock::ListClear(RpcObjectReference ref)
		{
			RequireListCallback()->ListClear(ref);
		}

		bool RpcControllerMock::ListContains(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListContains(ref, value);
		}

		vint RpcControllerMock::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListIndexOf(ref, value);
		}

		vint RpcControllerMock::DictGetCount(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetCount(ref);
		}

		Value RpcControllerMock::DictGet(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictGet(ref, key);
		}

		void RpcControllerMock::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			RequireListCallback()->DictSet(ref, key, value);
		}

		bool RpcControllerMock::DictRemove(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictRemove(ref, key);
		}

		void RpcControllerMock::DictClear(RpcObjectReference ref)
		{
			RequireListCallback()->DictClear(ref);
		}

		bool RpcControllerMock::DictContainsKey(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictContainsKey(ref, key);
		}

		RpcObjectReference RpcControllerMock::DictGetKeys(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetKeys(ref);
		}

		RpcObjectReference RpcControllerMock::DictGetValues(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetValues(ref);
		}

/***********************************************************************
* RpcControllerMock (IRpcListEventOps)
***********************************************************************/

		void RpcControllerMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			RequireListEventCallback()->OnItemChanged(ref, index, oldCount, newCount);
		}
	}
}
