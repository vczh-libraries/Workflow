/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::RPC

Lifecycle:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_RPC_LIFECYCLE
#define VCZH_WORKFLOW_LIBRARY_RPC_LIFECYCLE

#include "WfLibraryRpcController.h"

namespace vl
{
	namespace rpc_controller
	{
		class RpcLifecycleBase;

		class RpcLocalObjectTracker : public Object
		{
			friend class RpcLifecycleBase;
		private:
			RpcLifecycleBase*									lifecycle = nullptr;
			RpcObjectReference									ref;
		public:
			RpcLocalObjectTracker(RpcLifecycleBase* lc, RpcObjectReference r);
			~RpcLocalObjectTracker();
			void												Attach(RpcLifecycleBase* lc, RpcObjectReference r);
			void												Detach();
			RpcObjectReference									GetRef() const { return ref; }
			vint												GetClientId() const { return ref.clientId; }
			RpcLifecycleBase*									GetLifecycle() const { return lifecycle; }
			bool												IsTracked() const { return lifecycle != nullptr; }
		};

		class RpcWrapperTracker : public Object
		{
			friend class RpcLifecycleBase;
		private:
			RpcLifecycleBase*									lifecycle = nullptr;
			RpcObjectReference									ref;
		public:
			RpcWrapperTracker(RpcLifecycleBase* lc, RpcObjectReference r);
			~RpcWrapperTracker();
			void												Detach();
			RpcObjectReference									GetRef() const { return ref; }
			RpcLifecycleBase*									GetLifecycle() const { return lifecycle; }
		};

		struct RpcLocalObjectProperties : public Object
		{
			RpcObjectReference									ref;
			collections::SortedList<vint>						interestedClients;
			reflection::IDescriptable*							rawPtr = nullptr;
			Ptr<reflection::IDescriptable>						ownedPtr;
			Ptr<EventHandler>									eventHandler;
		};

		class RpcLifecycleBase : public Object, public IRpcLifecycle
		{
			friend class RpcLocalObjectTracker;
			friend class RpcWrapperTracker;
		public:
			using UniversalWrapperFactory = Func<Ptr<IRpcWrapperBase>(RpcObjectReference, IRpcLifecycle*)>;
		private:
			struct RpcWrapperProperties
			{
				reflection::DescriptableObject*					root = nullptr;
				IRpcWrapperBase*								proxy = nullptr;
			};
			using WrapperProperties = collections::Dictionary<RpcObjectReference, RpcWrapperProperties>;
		private:
			RpcControllerDefault								controller;
			vint												clientId = RpcClientId_Invalid;
			vint												nextObjectId = RpcObjectId_Invalid;
			collections::Dictionary<vint, Ptr<RpcLocalObjectProperties>>	localObjectProperties;
			IRpcDispatcher*										dispatcher = nullptr;
			static WString										InternalProperty_LocalObjectTracker;
			static WString										InternalProperty_WrapperTracker;
			UniversalWrapperFactory								universalWrapperFactory;
			WrapperProperties									wrapperProperties;

			void												TrackWrapper(reflection::DescriptableObject* root, IRpcWrapperBase* proxy, RpcObjectReference ref);
			void												UntrackWrapper(RpcObjectReference ref);
			bool												TryGetTrackedWrapperRef(reflection::DescriptableObject* obj, RpcObjectReference& ref)const;
			IRpcWrapperBase*									GetTrackedWrapper(RpcObjectReference ref)const;
			void												TrackLocalObject(RpcObjectReference ref, reflection::IDescriptable* obj);
			void												UntrackLocalObject(RpcObjectReference ref, bool clearInternalProperty);
			void												RemoveLocalObject(RpcObjectReference ref, bool clearInternalProperty);
			bool												IsTracked(vint objectId)const;
			Ptr<reflection::IDescriptable>						CreateCallerProxy(RpcObjectReference ref);
			void												DisconnectWrappersForFinalize();
		protected:
			collections::Dictionary<WString, vint>				idMap;

			virtual vint										DecideTypeId(reflection::IDescriptable* obj)const;
			virtual void										AttachLocalObjectEvents(RpcObjectReference ref, reflection::IDescriptable* obj) = 0;
		public:
			RpcLifecycleBase(vint _clientId);
			~RpcLifecycleBase();

			void												SetDispatcher(IRpcDispatcher* _dispatcher);
			void												SetIdMap(const collections::Dictionary<WString, vint>& _idMap);
			void												RegisterWrapperFactory(UniversalWrapperFactory factory);

			// IRpcLifecycle

			void												Finalize()override;
			vint												GetClientId()override;
			IRpcDispatcher*										GetDispatcher()override;
			RpcControllerDefault*								GetController()override;
			void												LocalObjectHold(RpcObjectReference ref, vint remoteClientId)override;
			void												LocalObjectUnhold(RpcObjectReference ref, vint remoteClientId)override;
			void												RegisterService(const WString& fullName, Ptr<reflection::IDescriptable> service)override;
			Ptr<reflection::IDescriptable>						RequestService(const WString& fullName)override;
			Ptr<reflection::IDescriptable>						RefToPtr(RpcObjectReference ref)override;
			RpcObjectReference									PtrToRef(Ptr<reflection::IDescriptable> obj)override;
		};
	}
}

#endif
