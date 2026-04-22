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
			friend class RpcDualLifecycleMock;
		private:
			class RpcDualLifecycleMock*									mock;
			rpc_controller::RpcObjectReference							ref;
		public:
			RpcDualObjectTracker(class RpcDualLifecycleMock* m, rpc_controller::RpcObjectReference r);
			~RpcDualObjectTracker();
			rpc_controller::RpcObjectReference							GetRef() const { return ref; }
			class RpcDualLifecycleMock*									GetMock() const { return mock; }
		};

		class RpcDualWrapperTracker : public Object
		{
			friend class RpcDualLifecycleMock;
		private:
			class RpcDualLifecycleMock*									mock;
			rpc_controller::IRpcWrapperBase*							proxy;
			rpc_controller::RpcObjectReference							ref;
		public:
			RpcDualWrapperTracker(class RpcDualLifecycleMock* m, rpc_controller::IRpcWrapperBase* p, rpc_controller::RpcObjectReference r);
			~RpcDualWrapperTracker();
			void														Detach();
			rpc_controller::RpcObjectReference							GetRef() const { return ref; }
			class RpcDualLifecycleMock*									GetMock() const { return mock; }
		};

		struct RpcLocalObjectProperties : public Object
		{
			rpc_controller::RpcObjectReference							ref;
			vint														refCount = 0;
			reflection::IDescriptable*									rawPtr = nullptr;
			Ptr<reflection::IDescriptable>								ownedPtr;
			Ptr<EventHandler>											eventHandler;
		};

		struct RpcWrapperEntry
		{
			reflection::DescriptableObject*								root = nullptr;
			rpc_controller::IRpcWrapperBase*							proxy = nullptr;
			rpc_controller::RpcObjectReference							ref;
		};

		class RpcDualObjectOps : public Object, public virtual rpc_controller::IRpcObjectOps
		{
		private:
			class RpcDualLifecycleMock*								owner = nullptr;
			Ptr<rpc_controller::IRpcObjectOps>					callback;
		public:
			RpcDualObjectOps(class RpcDualLifecycleMock* _owner, Ptr<rpc_controller::IRpcObjectOps> _callback);

			reflection::description::Value						InvokeMethod(rpc_controller::RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override;
			Ptr<reflection::description::IAsync>				InvokeMethodAsync(rpc_controller::RpcObjectReference ref, vint methodId, Ptr<reflection::description::IValueArray> arguments)override;
			void												ObjectHold(rpc_controller::RpcObjectReference ref, vint remoteClientId, bool hold)override;
			rpc_controller::RpcObjectReference					RequestService(vint typeId)override;
		};

		class RpcDualLifeCycleAdapter : public Object, public virtual rpc_controller::IRpcLifeCycle
		{
		private:
			class RpcDualLifecycleMock*									mock;
		public:
			RpcDualLifeCycleAdapter(class RpcDualLifecycleMock* _mock);
			~RpcDualLifeCycleAdapter();

			rpc_controller::IRpcController*								GetController()override;
			Ptr<reflection::IDescriptable>								RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference							PtrToRef(Ptr<reflection::IDescriptable> obj)override;
			void														RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service)override;
			Ptr<reflection::IDescriptable>								RequestService(const WString& fullName)override;
		};

		class RpcDualLifecycleMock : public RpcLifecycleMock
		{
			friend class RpcDualObjectTracker;
			friend class RpcDualWrapperTracker;
			friend class RpcDualLifeCycleAdapter;
		private:
			static WString																								InternalProperty_LocalObjectTracker;
			static WString																								InternalProperty_WrapperTracker;
			vint																										clientId = 1;
			vint																										nextObjectId = 1;
			Func<Ptr<rpc_controller::IRpcWrapperBase>(vint, rpc_controller::IRpcLifeCycle*)>			universalWrapperFactory;
			rpc_controller::RpcObjectReference													pendingProxyRef;
			collections::List<RpcWrapperEntry>													wrapperEntries;

			void																			TrackWrapper(reflection::DescriptableObject* root, rpc_controller::IRpcWrapperBase* proxy, rpc_controller::RpcObjectReference ref);
			void																			UntrackWrapper(rpc_controller::IRpcWrapperBase* proxy);
			bool																			TryGetTrackedWrapperRef(reflection::DescriptableObject* obj, rpc_controller::RpcObjectReference& ref)const;
			void																			TrackLocalObject(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj);
			void																			UntrackLocalObject(rpc_controller::RpcObjectReference ref);
			bool																			IsTracked(vint objectId)const;
			Ptr<reflection::IDescriptable>													CreateCallerProxy(rpc_controller::RpcObjectReference ref);
		protected:
			collections::Dictionary<vint, Ptr<RpcLocalObjectProperties>>								localObjectProps;
			collections::Dictionary<WString, vint>													idMap;

			virtual vint																		DecideTypeId(reflection::IDescriptable* obj)const;
			void																			DisconnectTrackedWrappers();
		public:
			RpcDualLifecycleMock(vint _clientId);
			~RpcDualLifecycleMock();

			void																	SetIdMap(const collections::Dictionary<WString, vint>& _idMap);
			void																	RegisterWrapperFactory(Func<Ptr<rpc_controller::IRpcWrapperBase>(vint, rpc_controller::IRpcLifeCycle*)> factory);

			// IRpcController

			void																	Register(Ptr<IRpcObjectOps> objectCallback, Ptr<IRpcObjectEventOps> eventCallback, Ptr<IRpcListOps> listCallback, Ptr<IRpcListEventOps> listEventCallback);
			rpc_controller::RpcObjectReference										RegisterLocalObject(vint typeId)override;
			void																	UnregisterLocalObject(rpc_controller::RpcObjectReference ref)override;
			void																	AcquireRemoteObject(rpc_controller::RpcObjectReference ref)override;
			void																	ReleaseRemoteObject(rpc_controller::RpcObjectReference ref)override;
			// IRpcObjectOps

			rpc_controller::RpcObjectReference										RequestService(vint typeId)override;
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
