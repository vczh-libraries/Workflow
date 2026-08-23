#ifndef VCZH_WORKFLOW_TEST_CASES_RPC_STDIO_DRIVER
#define VCZH_WORKFLOW_TEST_CASES_RPC_STDIO_DRIVER

#include "CppTypes.h"
#include "../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherClient.h"
#include "../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherServer.h"

namespace vl::rpc_controller_test
{
	using namespace vl::collections;
	using namespace vl::console;
	using namespace vl::glr::json;
	using namespace vl::inter_process;
	using namespace vl::inter_process::stdio_redirection;
	using namespace vl::reflection;
	using namespace vl::reflection::description;
	using namespace vl::rpc_controller;
	using namespace vl::rpc_controller::channeling;

	constexpr const wchar_t* RpcStdioTestChannelName = L"WorkflowRpcStdioTest";

	class RpcStdioTaskQueueThread : public Thread
	{
	private:
		Ptr<TaskQueue> taskQueue;

	protected:
		void Run() override;

	public:
		RpcStdioTaskQueueThread(Ptr<TaskQueue> _taskQueue);
	};

	class RpcStdioBroadcastingClient : public JsonLocalChannelClient
	{
	private:
		JsonChannelClient::ChannelMap				channelNames;
		Ptr<RpcJsonDispatcherServerForTaskQueue>	dispatcher;

	public:
		RpcStdioBroadcastingClient(Ptr<Parser> parser);

		const JsonChannelClient::ChannelNameList& OnGetChannelNames() override;
		vint Connect(JsonChannelServer* channelServer, Ptr<JsonChannelClient> self, Ptr<TaskQueue> taskQueue);
		RpcJsonDispatcherServer* GetDispatcher();
	};

	class RpcStdioChannelServer : public JsonNetworkChannelServer<StdioRedirectionServer>
	{
	private:
		SpinLock							lockState;
		EventObject						remoteConnected;
		SortedList<vint>					endpointClientIds;
		vint							remoteClientId = -1;
		RpcJsonDispatcherServer*			broadcastingDispatcher = nullptr;
		RpcJsonDispatcherClient*			requesterDispatcher = nullptr;
		bool							stopping = false;

	public:
		RpcStdioChannelServer(Ptr<Parser> parser);

		WaitForClientResult OnClientConnected(vint clientId, const JsonChannelClient::ChannelNameList& availableChannels, JsonChannelClient* localClient) override;
		void OnClientDisconnected(vint clientId) override;
		vint WaitForRemoteClient();
		void SetDispatchers(RpcJsonDispatcherServer* _broadcastingDispatcher, RpcJsonDispatcherClient* _requesterDispatcher);
		void BeginStopping();
	};

	template<typename TInstance, bool HasEvent>
	class RpcStdioDriverClient : public JsonLocalChannelClient
	{
	private:
		JsonChannelClient::ChannelMap		channelNames;
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
		RpcStdioDriverClient(Ptr<Parser> parser)
			: JsonLocalChannelClient(parser)
		{
			channelNames.Add(WString::Unmanaged(RpcStdioTestChannelName), nullptr);
		}

		const JsonChannelClient::ChannelNameList& OnGetChannelNames() override
		{
			return channelNames.Keys();
		}

		void OnConnected(vint clientId) override
		{
			CHECK_ERROR(dispatcher, L"RpcStdioDriverClient needs a dispatcher before connecting.");
			InitializeRpc(clientId);
		}

		vint Connect(JsonChannelServer* channelServer, Ptr<JsonChannelClient> self, Ptr<TaskQueue> taskQueue, const List<WString>& waitingForServices)
		{
			dispatcher = Ptr(new RpcJsonDispatcherClientForTaskQueue(taskQueue));
			return dispatcher->ConnectLocalServer(
				channelServer,
				self,
				GetChannels()[WString::Unmanaged(RpcStdioTestChannelName)],
				waitingForServices
				);
		}

		RpcJsonDispatcherClient* GetDispatcher()
		{
			CHECK_ERROR(dispatcher, L"RpcStdioDriverClient has not been connected.");
			return dispatcher.Obj();
		}
	};

	template<typename TInstance, bool HasEvent>
	void RunRpcStdioTestCase(
		const WString& itemName,
		const WString& serviceCommand,
		const List<WString>& waitingForServices
		)
	{
		Console::WriteLine(L"Rpc:" + itemName);

		auto parser = Ptr(new Parser);
		auto taskQueue = Ptr(new TaskQueue);
		auto taskQueueThread = Ptr(new RpcStdioTaskQueueThread(taskQueue));
		auto channelServer = Ptr(new RpcStdioChannelServer(parser));
		channelServer->Start();
		channelServer->ConnectNewClient(serviceCommand + L" \"" + itemName + L"\"");
		auto serviceClientId = channelServer->WaitForRemoteClient();
		CHECK_ERROR(serviceClientId == 1, L"The RPC stdio service must be assigned client ID 1.");

		auto requesterClient = Ptr(new RpcStdioDriverClient<TInstance, HasEvent>(parser));
		auto requesterClientId = requesterClient->Connect(channelServer.Obj(), requesterClient, taskQueue, waitingForServices);
		CHECK_ERROR(requesterClientId == 2, L"The RPC stdio driver must be assigned client ID 2.");

		auto broadcastingClient = Ptr(new RpcStdioBroadcastingClient(parser));
		auto broadcastingClientId = broadcastingClient->Connect(channelServer.Obj(), broadcastingClient, taskQueue);
		CHECK_ERROR(broadcastingClientId == 3, L"The RPC stdio dispatcher must be assigned client ID 3.");
		CHECK_ERROR(taskQueueThread->Start(), L"Failed to start the RPC stdio task queue.");

		auto requesterDispatcher = requesterClient->GetDispatcher();
		auto broadcastingDispatcher = broadcastingClient->GetDispatcher();
		requesterDispatcher->SetServerLocalClientId(broadcastingClientId);
		channelServer->SetDispatchers(broadcastingDispatcher, requesterDispatcher);
		requesterDispatcher->Initialize();

		auto actual = TInstance::Instance().clientMain(requesterDispatcher->GetRpcLifecycle());
		Console::WriteLine(L"    result : " + actual);

		EventObject finalized;
		CHECK_ERROR(finalized.CreateAutoUnsignal(false), L"Failed to create the RPC stdio finalization event.");
		taskQueue->QueueTask(Func<void()>([requesterDispatcher, &finalized]()
		{
			requesterDispatcher->FinalizeRpc();
			finalized.Signal();
		}));
		finalized.Wait();

		channelServer->BeginStopping();
		channelServer->Stop();
		taskQueue->QueueExitTask();
		taskQueueThread->Wait();
	}
}

extern void PrintRpcStdioSkippedTestCase(const vl::WString& itemName);
extern void RunTestCasesRpcStdio_Driver(const vl::WString& serviceCommand, const vl::collections::SortedList<vl::WString>& skippedTestCases);

#endif
