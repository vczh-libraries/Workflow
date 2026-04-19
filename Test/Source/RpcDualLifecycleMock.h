#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK

#include "RpcLifecycleMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		static constexpr vint RpcTypeId_NotFound = -100;
		class RpcDualObjectTracker : public Object
		{
		private:
			class RpcDualLifecycleMock*									mock;
			rpc_controller::RpcObjectReference							ref;
		public:
			RpcDualObjectTracker(class RpcDualLifecycleMock* m, rpc_controller::RpcObjectReference r);
			~RpcDualObjectTracker();
		};

		class RpcDualLifeCycleAdapter : public Object, public virtual rpc_controller::IRpcLifeCycle
		{
		private:
			class RpcDualLifecycleMock*									mock;
		public:
			RpcDualLifeCycleAdapter(class RpcDualLifecycleMock* _mock);

			rpc_controller::IRpcController*								GetController()override;
			Ptr<reflection::IDescriptable>								RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference							PtrToRef(Ptr<reflection::IDescriptable> obj)override;
			void														RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service)override;
			Ptr<reflection::IDescriptable>								RequestService(const WString& fullName)override;
		};

		class RpcDualLifecycleMock : public RpcLifecycleMock
		{
			friend class RpcDualObjectTracker;
			friend class RpcDualLifeCycleAdapter;
		protected:
			vint																																		clientId = 1;
			vint																																		nextObjectId = 1;
			RpcDualLifecycleMock*																														peer = nullptr;
			RpcDualLifeCycleAdapter*																													adapter = nullptr;
			collections::Dictionary<vint, vint>																											refCounts;
			collections::Dictionary<vint, vint>																											typeIds;
			collections::Dictionary<vint, reflection::IDescriptable*>																					localObjects;
			collections::Dictionary<vint, Ptr<reflection::IDescriptable>>																				ownedObjects;
			collections::Dictionary<const reflection::IDescriptable*, rpc_controller::RpcObjectReference>												refsByPtr;
			collections::Dictionary<vint, rpc_controller::RpcObjectReference>																			refsById;
			collections::Dictionary<vint, Func<Ptr<reflection::IDescriptable>(rpc_controller::IRpcLifeCycle*, rpc_controller::RpcObjectReference)>>		proxyFactories;
			collections::Dictionary<vint, reflection::description::IValueObservableList*>																observableProxies;
			collections::Dictionary<vint, Ptr<EventHandler>>																							eventHandlers;
			collections::Dictionary<WString, vint>																										idMap;
			collections::Dictionary<vint, Func<Ptr<reflection::IDescriptable>(rpc_controller::IRpcLifeCycle*)>>											wrapperFactories;
			Func<Ptr<reflection::IDescriptable>(vint, rpc_controller::IRpcLifeCycle*)>																	universalWrapperFactory;

			virtual vint																DecideTypeId(reflection::IDescriptable* obj)const;
			void																		TrackLocalObject(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj);
			void																		UntrackLocalObject(rpc_controller::RpcObjectReference ref);
			bool																		IsTracked(vint objectId)const;
			Ptr<reflection::IDescriptable>												CreateCallerProxy(rpc_controller::RpcObjectReference ref);
		public:
			RpcDualLifecycleMock(vint _clientId);
			~RpcDualLifecycleMock();

			void																	SetPeer(RpcDualLifecycleMock* _peer);
			void																	SetIdMap(const collections::Dictionary<WString, vint>& _idMap);
			void																	SetAdapter(RpcDualLifeCycleAdapter* _adapter);
			void																	RegisterWrapperFactory(vint typeId, Func<Ptr<reflection::IDescriptable>(rpc_controller::IRpcLifeCycle*)> factory);
			void																	RegisterWrapperFactory(Func<Ptr<reflection::IDescriptable>(vint, rpc_controller::IRpcLifeCycle*)> factory);

			// IRpcController

			void																	Register(Ptr<IRpcObjectOps> objectCallback, Ptr<IRpcObjectEventOps> eventCallback, Ptr<IRpcListOps> listCallback, Ptr<IRpcListEventOps> listEventCallback)override;
			rpc_controller::RpcObjectReference										RegisterLocalObject(vint typeId)override;
			void																	UnregisterLocalObject(rpc_controller::RpcObjectReference ref)override;
			void																	AcquireRemoteObject(rpc_controller::RpcObjectReference ref)override;
			void																	ReleaseRemoteObject(rpc_controller::RpcObjectReference ref)override;

			// IRpcLifeCycle

			Ptr<reflection::IDescriptable>											RequestService(const WString& fullName)override;
			Ptr<reflection::IDescriptable>											RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										PtrToRef(Ptr<reflection::IDescriptable> obj)override;

			// IRpcListEventOps

			void																	OnItemChanged(rpc_controller::RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};
	}
}

#endif
