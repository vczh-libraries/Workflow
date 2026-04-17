#include "RpcLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace reflection;
		using namespace reflection::description;
		using namespace rpc_controller;

/***********************************************************************
* RpcLifecycleMock (Require helpers)
***********************************************************************/

		IRpcObjectOps* RpcLifecycleMock::RequireObjectCallback()const
		{
			CHECK_ERROR(objectCallback != nullptr, L"RpcLifecycleMock requires an object callback.");
			return objectCallback;
		}

		IRpcObjectEventOps* RpcLifecycleMock::RequireEventCallback()const
		{
			CHECK_ERROR(eventCallback != nullptr, L"RpcLifecycleMock requires an event callback.");
			return eventCallback;
		}

		IRpcListOps* RpcLifecycleMock::RequireListCallback()const
		{
			CHECK_ERROR(listCallback != nullptr, L"RpcLifecycleMock requires a list callback.");
			return listCallback;
		}

		IRpcListEventOps* RpcLifecycleMock::RequireListEventCallback()const
		{
			CHECK_ERROR(listEventCallback != nullptr, L"RpcLifecycleMock requires a list event callback.");
			return listEventCallback;
		}

/***********************************************************************
* RpcLifecycleMock (IRpcIdSync)
***********************************************************************/

		void RpcLifecycleMock::SyncIds(Ptr<IValueDictionary> ids)
		{
			(void)ids;
		}

/***********************************************************************
* RpcLifecycleMock (IRpcController)
***********************************************************************/

		Ptr<IValueDictionary> RpcLifecycleMock::Register(Ptr<IRpcObjectOps> _objectCallback, Ptr<IRpcObjectEventOps> _eventCallback, Ptr<IRpcListOps> _listCallback, Ptr<IRpcListEventOps> _listEventCallback)
		{
			objectCallback = _objectCallback.Obj();
			eventCallback = _eventCallback.Obj();
			listCallback = _listCallback.Obj();
			listEventCallback = _listEventCallback.Obj();

			auto ids = IValueDictionary::Create();
			if (objectCallback) objectCallback->SyncIds(ids);
			if (eventCallback) eventCallback->SyncIds(ids);
			return ids;
		}

/***********************************************************************
* RpcLifecycleMock (IRpcLifeCycle)
***********************************************************************/

		IRpcController* RpcLifecycleMock::GetController()
		{
			return this;
		}

		void RpcLifecycleMock::RegisterService(const WString& fullName, Ptr<IDescriptable> service)
		{
			services.Set(fullName, service);
		}

		Ptr<IDescriptable> RpcLifecycleMock::RequestService(const WString& fullName)
		{
			if (auto index = services.Keys().IndexOf(fullName); index != -1)
			{
				return services.Values()[index];
			}
			return nullptr;
		}

/***********************************************************************
* RpcLifecycleMock (IRpcObjectOps)
***********************************************************************/

		Value RpcLifecycleMock::InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
			return RequireObjectCallback()->InvokeMethod(ref, methodId, arguments);
		}

		Ptr<IAsync> RpcLifecycleMock::InvokeMethodAsync(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)
		{
			return RequireObjectCallback()->InvokeMethodAsync(ref, methodId, arguments);
		}

		void RpcLifecycleMock::ObjectHold(RpcObjectReference ref, bool hold)
		{
			RequireObjectCallback()->ObjectHold(ref, hold);
		}

		RpcObjectReference RpcLifecycleMock::RequestService(vint typeId)
		{
			return RequireObjectCallback()->RequestService(typeId);
		}

/***********************************************************************
* RpcLifecycleMock (IRpcObjectEventOps)
***********************************************************************/

		void RpcLifecycleMock::InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)
		{
			RequireEventCallback()->InvokeEvent(ref, eventId, arguments);
		}

/***********************************************************************
* RpcLifecycleMock (IRpcListOps)
***********************************************************************/

		RpcObjectReference RpcLifecycleMock::EnumCreate(RpcObjectReference ref)
		{
			return RequireListCallback()->EnumCreate(ref);
		}

		bool RpcLifecycleMock::EnumNext(RpcObjectReference enumerator)
		{
			return RequireListCallback()->EnumNext(enumerator);
		}

		Value RpcLifecycleMock::EnumGetCurrent(RpcObjectReference enumerator)
		{
			return RequireListCallback()->EnumGetCurrent(enumerator);
		}

		vint RpcLifecycleMock::ListGetCount(RpcObjectReference ref)
		{
			return RequireListCallback()->ListGetCount(ref);
		}

		Value RpcLifecycleMock::ListGet(RpcObjectReference ref, vint index)
		{
			return RequireListCallback()->ListGet(ref, index);
		}

		void RpcLifecycleMock::ListSet(RpcObjectReference ref, vint index, const Value& value)
		{
			RequireListCallback()->ListSet(ref, index, value);
		}

		vint RpcLifecycleMock::ListAdd(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListAdd(ref, value);
		}

		vint RpcLifecycleMock::ListInsert(RpcObjectReference ref, vint index, const Value& value)
		{
			return RequireListCallback()->ListInsert(ref, index, value);
		}

		bool RpcLifecycleMock::ListRemoveAt(RpcObjectReference ref, vint index)
		{
			return RequireListCallback()->ListRemoveAt(ref, index);
		}

		void RpcLifecycleMock::ListClear(RpcObjectReference ref)
		{
			RequireListCallback()->ListClear(ref);
		}

		bool RpcLifecycleMock::ListContains(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListContains(ref, value);
		}

		vint RpcLifecycleMock::ListIndexOf(RpcObjectReference ref, const Value& value)
		{
			return RequireListCallback()->ListIndexOf(ref, value);
		}

		vint RpcLifecycleMock::DictGetCount(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetCount(ref);
		}

		Value RpcLifecycleMock::DictGet(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictGet(ref, key);
		}

		void RpcLifecycleMock::DictSet(RpcObjectReference ref, const Value& key, const Value& value)
		{
			RequireListCallback()->DictSet(ref, key, value);
		}

		bool RpcLifecycleMock::DictRemove(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictRemove(ref, key);
		}

		void RpcLifecycleMock::DictClear(RpcObjectReference ref)
		{
			RequireListCallback()->DictClear(ref);
		}

		bool RpcLifecycleMock::DictContainsKey(RpcObjectReference ref, const Value& key)
		{
			return RequireListCallback()->DictContainsKey(ref, key);
		}

		RpcObjectReference RpcLifecycleMock::DictGetKeys(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetKeys(ref);
		}

		RpcObjectReference RpcLifecycleMock::DictGetValues(RpcObjectReference ref)
		{
			return RequireListCallback()->DictGetValues(ref);
		}

/***********************************************************************
* RpcLifecycleMock (IRpcListEventOps)
***********************************************************************/

		void RpcLifecycleMock::OnItemChanged(RpcObjectReference ref, vint index, vint oldCount, vint newCount)
		{
			RequireListEventCallback()->OnItemChanged(ref, index, oldCount, newCount);
		}
	}
}
