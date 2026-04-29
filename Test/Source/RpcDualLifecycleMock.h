#ifndef VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK
#define VCZH_WORKFLOW_TEST_RPC_DUAL_LIFECYCLE_MOCK

#include "RpcControllerMock.h"

namespace vl
{
	namespace rpc_controller_test
	{
		static constexpr vint RpcTypeId_NotFound = -100;

		class RpcDualLifecycleMock;
		class RpcDualControllerMock;

		struct RpcDualEventDispatch
		{
			rpc_controller::RpcObjectReference				ref;
			vint											eventId = 0;

			auto operator<=>(const RpcDualEventDispatch&) const = default;
		};

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
			vint																	clientId = 1;
			vint																	nextObjectId = 1;
			collections::Dictionary<vint, Ptr<RpcLocalObjectProperties>>			localObjectProperties;
		public:
			RpcDualControllerMock(RpcDualLifecycleMock* lc, vint _clientId);
			~RpcDualControllerMock();

			vint																	GetClientId()const;
			const collections::Dictionary<vint, Ptr<RpcLocalObjectProperties>>&		GetLocalObjectProperties()const;
			void																	UnregisterAllLocalObjects();

			// IRpcController

			void																	Register(Ptr<IRpcObjectOps> objectCallback, Ptr<IRpcObjectEventOps> eventCallback, Ptr<IRpcListOps> listCallback, Ptr<IRpcListEventOps> listEventCallback)override;
			rpc_controller::RpcObjectReference										RegisterLocalObject(vint typeId)override;
			void																	UnregisterLocalObject(rpc_controller::RpcObjectReference ref)override;
			void																	AcquireRemoteObject(rpc_controller::RpcObjectReference ref)override;
			void																	ReleaseRemoteObject(rpc_controller::RpcObjectReference ref)override;

			// IRpcObjectEventOps

			void																	InvokeEvent(rpc_controller::RpcObjectReference ref, vint eventId, Ptr<reflection::description::IValueArray> arguments)override;

			// IRpcListEventOps

			void																	OnItemChanged(rpc_controller::RpcObjectReference ref, vint index, vint oldCount, vint newCount)override;
		};

		class RpcDualLifecycleMock : public Object, public rpc_controller::IRpcLifeCycle
		{
			friend class RpcDualControllerMock;
			friend class RpcDualLocalObjectTracker;
			friend class RpcDualWrapperTracker;
			using UniversalWrapperFactory = Func<Ptr<rpc_controller::IRpcWrapperBase>(rpc_controller::RpcObjectReference, rpc_controller::IRpcLifeCycle*)>;
		private:
			RpcDualControllerMock													controller;
			static WString															InternalProperty_LocalObjectTracker;
			static WString															InternalProperty_WrapperTracker;
			collections::SortedList<RpcDualEventDispatch>							forwardingEvents;
			UniversalWrapperFactory													universalWrapperFactory;
			Ptr<rpc_controller::IRpcObjectOps>										localObjectCallback;
			collections::Dictionary<rpc_controller::RpcObjectReference, RpcWrapperProperties>	wrapperProperties;

			bool																	BeginForwardingEvent(rpc_controller::RpcObjectReference ref, vint eventId);
			void																	EndForwardingEvent(rpc_controller::RpcObjectReference ref, vint eventId);
			void																	TrackWrapper(reflection::DescriptableObject* root, rpc_controller::IRpcWrapperBase* proxy, rpc_controller::RpcObjectReference ref);
			void																	UntrackWrapper(rpc_controller::RpcObjectReference ref);
			bool																	TryGetTrackedWrapperRef(reflection::DescriptableObject* obj, rpc_controller::RpcObjectReference& ref)const;
			rpc_controller::IRpcWrapperBase*										GetTrackedWrapper(rpc_controller::RpcObjectReference ref)const;
			bool																	HasEventTarget(rpc_controller::RpcObjectReference ref)const;
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
			void																	RegisterLocalObjectOps(Ptr<rpc_controller::IRpcObjectOps> objectCallback);

			// IRpcLifeCycle

			RpcDualControllerMock*													GetController()override;
			void																	RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service)override;
			Ptr<reflection::IDescriptable>											RequestService(const WString& fullName)override;
			Ptr<reflection::IDescriptable>											RefToPtr(rpc_controller::RpcObjectReference ref)override;
			rpc_controller::RpcObjectReference										PtrToRef(Ptr<reflection::IDescriptable> obj)override;
		};
	}
}

#endif
