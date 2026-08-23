#ifndef VCZH_WORKFLOW_TEST_CASES_RPC_STDIO_SERVICE
#define VCZH_WORKFLOW_TEST_CASES_RPC_STDIO_SERVICE

#include "CppTypes.h"
#include "../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherClient.h"

namespace vl::rpc_controller_test
{
	using namespace vl::collections;
	using namespace vl::glr::json;
	using namespace vl::inter_process;
	using namespace vl::inter_process::stdio_redirection;
	using namespace vl::reflection;
	using namespace vl::reflection::description;
	using namespace vl::rpc_controller;
	using namespace vl::rpc_controller::channeling;

	constexpr const wchar_t* RpcStdioServiceChannelName = L"WorkflowRpcStdioTest";

	template<typename TInstance, bool HasEvent>
	class RpcStdioServiceClient : public JsonNetworkChannelClient
	{
	private:
		JsonChannelClient::ChannelMap		channelNames;
		Ptr<TaskQueue>						taskQueue;
		Ptr<RpcJsonDispatcherClientForTaskQueue>	dispatcher;

		void InitializeRpc(vint clientId)
		{
			auto rpcDispatcher = Ptr(new RpcJsonDispatcher(clientId, dispatcher.Obj()));
			auto lifecycle = Ptr(new RpcJsonLifecycle(clientId, rpcDispatcher.Obj()));
			dispatcher->SetRpcObjects(rpcDispatcher, lifecycle);

			auto& instance = TInstance::Instance();
			auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(instance.rpc_GetIds()));
			lifecycle->SetIdMap(idMap.Ref());

			auto serializer = instance.rpcops_IRpcSerializer();
			auto objectOps = instance.rpcops_IRpcObjectOpsJson(lifecycle.Obj());
			auto objectEventOps = instance.rpcops_IRpcObjectEventOpsJson(lifecycle.Obj());
			auto ops = instance.rpcops_IOps_CreateJson(lifecycle.Obj());

			Func<void(RpcObjectReference, IDescriptable*)> eventAttacher;
			if constexpr (HasEvent)
			{
				eventAttacher = Func<void(RpcObjectReference, IDescriptable*)>([this, ops](RpcObjectReference ref, IDescriptable* obj)
				{
					if (ref.typeId < 0) return;
					TInstance::Instance().rpclistener_Attach(ref.typeId, dispatcher->GetRpcLifecycle(), ref, obj, ops);
				});
			}

			lifecycle->Register(
				serializer,
				objectOps,
				objectEventOps,
				Func<vint(IDescriptable*)>([](IDescriptable* obj)
				{
					return TInstance::Instance().rpcwrapper_GetTypeId(BoxValue<IDescriptable*>(obj));
				}),
				eventAttacher
				);
			lifecycle->RegisterWrapperFactory(Func<Ptr<IRpcWrapperBase>(RpcObjectReference, IRpcLifecycle*)>(
				[ops](RpcObjectReference ref, IRpcLifecycle* lc)
				{
					return TInstance::Instance().rpcwrapper_Create(ref, lc, ops);
				}));
		}

	public:
		RpcStdioServiceClient(Ptr<INetworkProtocolClient> networkClient, Ptr<Parser> parser, Ptr<TaskQueue> _taskQueue)
			: JsonNetworkChannelClient(networkClient, parser)
			, taskQueue(_taskQueue)
			, dispatcher(Ptr(new RpcJsonDispatcherClientForTaskQueue(_taskQueue)))
		{
			channelNames.Add(WString::Unmanaged(RpcStdioServiceChannelName), nullptr);
		}

		const JsonChannelClient::ChannelNameList& OnGetChannelNames() override
		{
			return channelNames.Keys();
		}

		void OnConnected(vint clientId) override
		{
			InitializeRpc(clientId);
		}

		void OnDisconnected() override
		{
			taskQueue->QueueExitTask();
		}

		void OnReadError(const WString&) override
		{
			CHECK_FAIL(L"The RPC stdio service received an invalid protocol message.");
		}

		void OnLocalError(const WString&, bool) override
		{
			CHECK_FAIL(L"The RPC stdio service encountered a local protocol error.");
		}

		void Run()
		{
			List<WString> waitingForServices;
			dispatcher->WaitForServer(
				this,
				GetChannels()[WString::Unmanaged(RpcStdioServiceChannelName)],
				waitingForServices
				);
			TInstance::Instance().serviceMain(dispatcher->GetRpcLifecycle());
			dispatcher->Initialize();
			taskQueue->RunTaskQueue();
			dispatcher->FinalizeRpc();
		}
	};

	template<typename TInstance, bool HasEvent>
	void RunRpcStdioService()
	{
		auto parser = Ptr(new Parser);
		auto taskQueue = Ptr(new TaskQueue);
		auto networkClient = Ptr(new StdioRedirectionClient);
		auto channelClient = Ptr(new RpcStdioServiceClient<TInstance, HasEvent>(networkClient, parser, taskQueue));
		channelClient->Run();
	}

}

extern void RunTestCasesRpcStdio_Service(const vl::WString& itemName);

#endif
