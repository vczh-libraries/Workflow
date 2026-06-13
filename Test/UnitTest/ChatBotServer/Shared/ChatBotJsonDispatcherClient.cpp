#include "ChatBotJsonDispatcherClient.h"

namespace chatbot
{
	using namespace vl;
	using namespace vl::collections;
	using namespace vl::reflection;
	using namespace vl::reflection::description;
	using namespace vl::rpc_controller;
	using namespace vl::rpc_controller::channeling;

	ChatBotJsonDispatcherClient::ChatBotJsonDispatcherClient(Ptr<TaskQueue> _taskQueue)
		: RpcJsonDispatcherClientForTaskQueue(_taskQueue)
	{
	}

	void ChatBotJsonDispatcherClient::InitializeRpc(vint clientId)
	{
		auto& app = vl_workflow_global::ChatBotApp::Instance();
		auto rpcDispatcher = Ptr(new RpcJsonDispatcher(clientId, this));
		auto lifecycle = Ptr(new RpcJsonLifecycle(clientId, rpcDispatcher.Obj()));
		SetRpcObjects(rpcDispatcher, lifecycle);

		auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(app.rpc_GetIds()));
		lifecycle->SetIdMap(idMap.Ref());

		auto serializer = app.rpcops_IRpcSerializer();
		auto objectOps = app.rpcops_IRpcObjectOpsJson(lifecycle.Obj());
		auto objectEventOps = app.rpcops_IRpcObjectEventOpsJson(lifecycle.Obj());
		auto ops = app.rpcops_IOps_CreateJson(lifecycle.Obj());

		lifecycle->Register(
			serializer,
			objectOps,
			objectEventOps,
			Func<vint(IDescriptable*)>([](IDescriptable* obj)
			{
				return vl_workflow_global::ChatBotApp::Instance().rpcwrapper_GetTypeId(BoxValue<IDescriptable*>(obj));
			}),
			Func<void(RpcObjectReference, IDescriptable*)>([this, ops](RpcObjectReference ref, IDescriptable* obj)
			{
				vl_workflow_global::ChatBotApp::Instance().rpclistener_Attach(ref.typeId, GetRpcJsonLifecycle(), ref, obj, ops);
			})
			);

		lifecycle->RegisterWrapperFactory(Func<Ptr<IRpcWrapperBase>(RpcObjectReference, IRpcLifecycle*)>(
			[ops](RpcObjectReference ref, IRpcLifecycle* lc)
			{
				return vl_workflow_global::ChatBotApp::Instance().rpcwrapper_Create(ref, lc, ops);
			}));
	}
}
