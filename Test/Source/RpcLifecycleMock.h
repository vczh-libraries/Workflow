#ifndef VCZH_WORKFLOW_TEST_RPC_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_LIFECYCLE_MOCK

#include "../../Source/Library/WfLibraryRpc.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcLifecycleMock : public Object, public rpc_controller::IRpcLifeCycle, public rpc_controller::IRpcController
		{
		protected:
			IRpcObjectOps*																	objectCallback = nullptr;
			IRpcObjectEventOps*																eventCallback = nullptr;
			IRpcListOps*																	listCallback = nullptr;
			IRpcListEventOps*																listEventCallback = nullptr;
			collections::Dictionary<WString, Ptr<reflection::IDescriptable>>				services;

			IRpcObjectOps*																	RequireObjectCallback()const;
			IRpcObjectEventOps*																RequireEventCallback()const;
			IRpcListOps*																	RequireListCallback()const;
			IRpcListEventOps*																RequireListEventCallback()const;
		public:

			// IRpcIdSync

			void																			SyncIds(Ptr<reflection::description::IValueDictionary> ids)override;

			// IRpcController

			Ptr<reflection::description::IValueDictionary>									Register(Ptr<IRpcObjectOps> objectCallback, Ptr<IRpcObjectEventOps> eventCallback, Ptr<IRpcListOps> listCallback, Ptr<IRpcListEventOps> listEventCallback)override;

			// IRpcLifeCycle

			Ptr<rpc_controller::IRpcController>												GetController()const override;
			void																			RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service)override;
			Ptr<reflection::IDescriptable>													RequestService(const WString& fullName)override;

			// IRpcObjectOps

			reflection::description::Value													InvokeMethod(rpc_controller::RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override;
			Ptr<reflection::description::IAsync>											InvokeMethodAsync(rpc_controller::RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override;
			void																			ObjectHold(rpc_controller::RpcObjectReference ref, bool hold)override;
			rpc_controller::RpcObjectReference												RequestService(vint typeId)override;

			// IRpcObjectEventOps

			void																			InvokeEvent(rpc_controller::RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments)override;

			// IRpcListOps

			rpc_controller::RpcObjectReference												EnumCreate(rpc_controller::RpcObjectReference ref)override;
			bool																			EnumNext(rpc_controller::RpcObjectReference enumerator)override;
			reflection::description::Value													EnumGetCurrent(rpc_controller::RpcObjectReference enumerator)override;
			vint																			ListGetCount(rpc_controller::RpcObjectReference ref)override;
			reflection::description::Value													ListGet(rpc_controller::RpcObjectReference ref, vint index)override;
			void																			ListSet(rpc_controller::RpcObjectReference ref, vint index, const reflection::description::Value& value)override;
			vint																			ListAdd(rpc_controller::RpcObjectReference ref, const reflection::description::Value& value)override;
			vint																			ListInsert(rpc_controller::RpcObjectReference ref, vint index, const reflection::description::Value& value)override;
			bool																			ListRemoveAt(rpc_controller::RpcObjectReference ref, vint index)override;
			void																			ListClear(rpc_controller::RpcObjectReference ref)override;
			bool																			ListContains(rpc_controller::RpcObjectReference ref, const reflection::description::Value& value)override;
			vint																			ListIndexOf(rpc_controller::RpcObjectReference ref, const reflection::description::Value& value)override;
			vint																			DictGetCount(rpc_controller::RpcObjectReference ref)override;
			reflection::description::Value													DictGet(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key)override;
			void																			DictSet(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key, const reflection::description::Value& value)override;
			bool																			DictRemove(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key)override;
			void																			DictClear(rpc_controller::RpcObjectReference ref)override;
			bool																			DictContainsKey(rpc_controller::RpcObjectReference ref, const reflection::description::Value& key)override;
			rpc_controller::RpcObjectReference												DictGetKeys(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference												DictGetValues(rpc_controller::RpcObjectReference ref)override;

			// IRpcListEventOps

			void																			OnItemChanged(rpc_controller::RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};
	}
}

#endif
