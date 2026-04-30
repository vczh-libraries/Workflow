#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK

#include "RpcControllerMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		class RpcDualLifecycleMock;
		class RpcDualControllerMock;
		class RpcDualDispatcherMock;

		class RpcDualLocalObjectTracker : public Object
		{
			friend class RpcDualLifecycleMock;
		private:
			class RpcDualLifecycleMock* 					mock = nullptr;
			rpc_controller::RpcObjectReference				ref;
		public:
			RpcDualLocalObjectTracker(class RpcDualLifecycleMock* m, rpc_controller::RpcObjectReference r);
			~RpcDualLocalObjectTracker();
			void											Attach(class RpcDualLifecycleMock* m, rpc_controller::RpcObjectReference r);
			void											Detach();
			rpc_controller::RpcObjectReference				GetRef() const { return ref; }
			vint											GetClientId() const { return ref.clientId; }
			class RpcDualLifecycleMock*						GetMock() const { return mock; }
			bool											IsTracked() const { return mock != nullptr; }
		};

		class RpcDualWrapperTracker : public Object
		{
			friend class RpcDualLifecycleMock;
		private:
			class RpcDualLifecycleMock*							mock;
			rpc_controller::RpcObjectReference					ref;
		public:
			RpcDualWrapperTracker(class RpcDualLifecycleMock* m, rpc_controller::IRpcWrapperBase* p, rpc_controller::RpcObjectReference r);
			~RpcDualWrapperTracker();
			void												Detach();
			rpc_controller::RpcObjectReference					GetRef() const { return ref; }
			class RpcDualLifecycleMock*							GetMock() const { return mock; }
		};

		struct RpcLocalObjectProperties : public Object
		{
			rpc_controller::RpcObjectReference					ref;
			vint												refCount = 0;
			reflection::IDescriptable*							rawPtr = nullptr;
			Ptr<reflection::IDescriptable>						ownedPtr;
			Ptr<EventHandler>									eventHandler;
			collections::List<Func<void()>>						nativeEventDetachments;
		};

		struct RpcWrapperProperties
		{
			reflection::DescriptableObject*						root = nullptr;
			rpc_controller::IRpcWrapperBase*					proxy = nullptr;
		};

		class RpcDualControllerMock : public RpcControllerMock
		{
		private:
			RpcDualLifecycleMock*													lifecycle = nullptr;
			vint																	clientId = rpc_controller::RpcClientId_Invalid;
			vint																	nextObjectId = rpc_controller::RpcObjectId_Invalid;
			collections::Dictionary<vint, Ptr<RpcLocalObjectProperties>>			localObjectProperties;
		public:
			RpcDualControllerMock(RpcDualLifecycleMock* lc, vint _clientId);
			~RpcDualControllerMock();

			vint																	GetClientId()const;
			const collections::Dictionary<vint, Ptr<RpcLocalObjectProperties>>&		GetLocalObjectProperties()const;
			void																	UnregisterAllLocalObjects(bool includeRegisteredServices = false);

			// IRpcController

			void																	Register(Ptr<rpc_controller::IRpcObjectOps> objectCallback, Ptr<rpc_controller::IRpcObjectEventOps> eventCallback, Ptr<rpc_controller::IRpcListOps> listCallback, Ptr<rpc_controller::IRpcListEventOps> listEventCallback)override;
			rpc_controller::RpcObjectReference										RegisterLocalObject(vint typeId)override;
			void																	UnregisterLocalObject(rpc_controller::RpcObjectReference ref)override;
			void																	AcquireRemoteObject(rpc_controller::RpcObjectReference ref)override;
			void																	ReleaseRemoteObject(rpc_controller::RpcObjectReference ref)override;
		};

		class RpcDualDispatcherMock : public Object, public rpc_controller::IRpcDispatcher
		{
		private:
			RpcDualLifecycleMock*													lifecycle1 = nullptr;
			RpcDualLifecycleMock*													lifecycle2 = nullptr;
			collections::Dictionary<vint, rpc_controller::RpcObjectReference>		services;

			RpcDualLifecycleMock*													GetLifecycle(vint clientId)const;
			RpcDualLifecycleMock*													GetOtherLifecycle(vint clientId)const;
		public:
			RpcDualDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2);
			~RpcDualDispatcherMock();

			bool																	IsRegisteredService(rpc_controller::RpcObjectReference ref)override;
			void																	RegisterService(vint typeId, rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										RequestService(vint typeId)override;
			rpc_controller::IRpcListEventOps*										BroadcastFromClient_ListEventOps(vint selfClientId)override;
			rpc_controller::IRpcObjectEventOps*										BroadcastFromClient_ObjectEventOps(vint selfClientId)override;
			rpc_controller::IRpcListOps*											SendToClient_ListOps(vint targetClientId)override;
			rpc_controller::IRpcObjectOps*											SendToClient_ObjectOps(vint targetClientId)override;
		};

		class RpcDualLifecycleMock : public Object, public rpc_controller::IRpcLifeCycle
		{
			friend class RpcDualControllerMock;
			friend class RpcDualDispatcherMock;
			friend class RpcDualLocalObjectTracker;
			friend class RpcDualWrapperTracker;
			using UniversalWrapperFactory = Func<Ptr<rpc_controller::IRpcWrapperBase>(rpc_controller::RpcObjectReference, rpc_controller::IRpcLifeCycle*)>;
			using WrapperProperties = collections::Dictionary<rpc_controller::RpcObjectReference, RpcWrapperProperties>;
		private:
			RpcDualControllerMock													controller;
			rpc_controller::IRpcDispatcher*											dispatcher = nullptr;
			static WString															InternalProperty_LocalObjectTracker;
			static WString															InternalProperty_WrapperTracker;
			UniversalWrapperFactory													universalWrapperFactory;
			WrapperProperties														wrapperProperties;

			void																	TrackWrapper(reflection::DescriptableObject* root, rpc_controller::IRpcWrapperBase* proxy, rpc_controller::RpcObjectReference ref);
			void																	UntrackWrapper(rpc_controller::RpcObjectReference ref);
			bool																	TryGetTrackedWrapperRef(reflection::DescriptableObject* obj, rpc_controller::RpcObjectReference& ref)const;
			rpc_controller::IRpcWrapperBase*										GetTrackedWrapper(rpc_controller::RpcObjectReference ref)const;
			void																	TrackLocalObject(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj);
			void																	UntrackLocalObject(rpc_controller::RpcObjectReference ref);
			bool																	IsTracked(vint objectId)const;
			Ptr<reflection::IDescriptable>											CreateCallerProxy(rpc_controller::RpcObjectReference ref);
		protected:
			collections::Dictionary<WString, vint>									idMap;

			virtual vint															DecideTypeId(reflection::IDescriptable* obj)const;
			virtual bool															AttachLocalObjectEvents(rpc_controller::RpcObjectReference ref, reflection::IDescriptable* obj, collections::List<Func<void()>>& detachments);
			void																	DisconnectTrackedWrappers();
		public:
			RpcDualLifecycleMock(vint _clientId);
			~RpcDualLifecycleMock();

			void																	SetIdMap(const collections::Dictionary<WString, vint>& _idMap);
			void																	RegisterWrapperFactory(UniversalWrapperFactory factory);

			// IRpcLifeCycle

			vint																	GetClientId()override;
			rpc_controller::IRpcDispatcher*											GetDispatcher()override;
			RpcDualControllerMock*													GetController()override;
			void																	RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service)override;
			Ptr<reflection::IDescriptable>											RequestService(const WString& fullName)override;
			Ptr<reflection::IDescriptable>											RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										PtrToRef(Ptr<reflection::IDescriptable> obj)override;
		};
	}
}

#endif
