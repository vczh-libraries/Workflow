#include "../../Source/TestCasesRpcStdio_Driver.h"

#ifdef VCZH_MSVC
#include <crtdbg.h>
#endif

namespace vl::rpc_controller_test
{
	using namespace vl::filesystem;

	void RpcStdioTaskQueueThread::Run()
	{
		taskQueue->RunTaskQueue();
	}

	RpcStdioTaskQueueThread::RpcStdioTaskQueueThread(Ptr<TaskQueue> _taskQueue)
		: taskQueue(_taskQueue)
	{
		CHECK_ERROR(taskQueue, L"RpcStdioTaskQueueThread needs a task queue.");
	}

	RpcStdioBroadcastingClient::RpcStdioBroadcastingClient(Ptr<Parser> parser)
		: JsonLocalChannelClient(parser)
	{
		channelNames.Add(WString::Unmanaged(RpcStdioTestChannelName), nullptr);
	}

	const JsonChannelClient::ChannelNameList& RpcStdioBroadcastingClient::OnGetChannelNames()
	{
		return channelNames.Keys();
	}

	vint RpcStdioBroadcastingClient::Connect(JsonChannelServer* channelServer, Ptr<JsonChannelClient> self, Ptr<TaskQueue> taskQueue)
	{
		CHECK_ERROR(channelServer, L"RpcStdioBroadcastingClient needs a channel server.");
		CHECK_ERROR(self, L"RpcStdioBroadcastingClient needs itself as a shared pointer.");
		auto clientId = channelServer->ConnectLocalClient(self);
		CHECK_ERROR(clientId != -1, L"RpcStdioBroadcastingClient failed to connect.");
		dispatcher = Ptr(new RpcJsonDispatcherServerForTaskQueue(
			this,
			GetChannels()[WString::Unmanaged(RpcStdioTestChannelName)],
			taskQueue
			));
		return clientId;
	}

	RpcJsonDispatcherServer* RpcStdioBroadcastingClient::GetDispatcher()
	{
		CHECK_ERROR(dispatcher, L"RpcStdioBroadcastingClient has not been connected.");
		return dispatcher.Obj();
	}

	RpcStdioChannelServer::RpcStdioChannelServer(Ptr<Parser> parser)
		: JsonNetworkChannelServer<StdioRedirectionServer>(parser)
	{
		CHECK_ERROR(remoteConnected.CreateAutoUnsignal(false), L"Failed to create the RPC stdio connection event.");
	}

	WaitForClientResult RpcStdioChannelServer::OnClientConnected(vint clientId, const JsonChannelClient::ChannelNameList& availableChannels, JsonChannelClient* localClient)
	{
		if (availableChannels.Count() != 1 || !availableChannels.Contains(WString::Unmanaged(RpcStdioTestChannelName)))
		{
			return WaitForClientResult::Reject;
		}
		if (localClient && dynamic_cast<RpcStdioBroadcastingClient*>(localClient))
		{
			return WaitForClientResult::Accept;
		}

		bool signalRemote = false;
		SPIN_LOCK(lockState)
		{
			CHECK_ERROR(!broadcastingDispatcher, L"RPC stdio endpoints cannot connect after dispatching begins.");
			CHECK_ERROR(!endpointClientIds.Contains(clientId), L"The RPC stdio endpoint client ID is duplicated.");
			endpointClientIds.Add(clientId);
			if (!localClient)
			{
				CHECK_ERROR(remoteClientId == -1, L"Only one RPC stdio service can connect.");
				remoteClientId = clientId;
				signalRemote = true;
			}
		}
		if (signalRemote)
		{
			remoteConnected.Signal();
		}
		return WaitForClientResult::Accept;
	}

	void RpcStdioChannelServer::OnClientDisconnected(vint clientId)
	{
		RpcJsonDispatcherServer* currentBroadcastingDispatcher = nullptr;
		RpcJsonDispatcherClient* currentRequesterDispatcher = nullptr;
		bool remoteLost = false;
		SPIN_LOCK(lockState)
		{
			if (endpointClientIds.Contains(clientId))
			{
				endpointClientIds.Remove(clientId);
				currentBroadcastingDispatcher = broadcastingDispatcher;
			}
			remoteLost = !stopping && clientId == remoteClientId;
			currentRequesterDispatcher = requesterDispatcher;
		}
		if (currentBroadcastingDispatcher)
		{
			currentBroadcastingDispatcher->DisconnectClient(clientId);
		}
		if (remoteLost && currentRequesterDispatcher)
		{
			currentRequesterDispatcher->InjectException(L"The RPC stdio service disconnected unexpectedly.");
		}
	}

	vint RpcStdioChannelServer::WaitForRemoteClient()
	{
		CHECK_ERROR(remoteConnected.WaitForTime(30000), L"Timed out waiting for the RPC stdio service to connect.");
		vint result = -1;
		SPIN_LOCK(lockState)
		{
			result = remoteClientId;
		}
		return result;
	}

	void RpcStdioChannelServer::SetDispatchers(RpcJsonDispatcherServer* _broadcastingDispatcher, RpcJsonDispatcherClient* _requesterDispatcher)
	{
		CHECK_ERROR(_broadcastingDispatcher, L"RpcStdioChannelServer needs a broadcasting dispatcher.");
		CHECK_ERROR(_requesterDispatcher, L"RpcStdioChannelServer needs a requester dispatcher.");
		List<vint> clientIds;
		SPIN_LOCK(lockState)
		{
			CHECK_ERROR(!broadcastingDispatcher && !requesterDispatcher, L"RpcStdioChannelServer dispatchers can only be set once.");
			broadcastingDispatcher = _broadcastingDispatcher;
			requesterDispatcher = _requesterDispatcher;
			CopyFrom(clientIds, endpointClientIds);
		}
		for (auto clientId : clientIds)
		{
			broadcastingDispatcher->RegisterClient(clientId);
		}
	}

	void RpcStdioChannelServer::BeginStopping()
	{
		SPIN_LOCK(lockState)
		{
			stopping = true;
		}
	}

}

void PrintRpcStdioSkippedTestCase(const vl::WString& itemName)
{
	vl::console::Console::WriteLine(L"[SKIPPED] Rpc:" + itemName);
}

using namespace vl;
using namespace vl::collections;
using namespace vl::filesystem;
using namespace vl::rpc_controller_test;

#if defined VCZH_MSVC
int wmain(int argc, wchar_t* argv[])
#elif defined VCZH_GCC
int main(int argc, char* argv[])
#endif
{
	CHECK_ERROR(argc == 2 || argc == 3, L"Usage: RpcStdioTest_Driver <cli-command-to-start-RpcStdioTest_Service> [path-to-SkippedTestCaseListFile]");

#if defined VCZH_MSVC
	auto serviceCommand = WString::Unmanaged(argv[1]);
#elif defined VCZH_GCC
	auto serviceCommand = atow(AString::Unmanaged(argv[1]));
#endif
	SortedList<WString> skippedTestCases;
	if (argc == 3)
	{
#if defined VCZH_MSVC
		auto skippedTestCaseListFile = WString::Unmanaged(argv[2]);
#elif defined VCZH_GCC
		auto skippedTestCaseListFile = atow(AString::Unmanaged(argv[2]));
#endif
		List<WString> lines;
		CHECK_ERROR(File(skippedTestCaseListFile).ReadAllLinesByBom(lines), L"Failed to read the skipped RPC stdio test case list.");
		for (auto line : lines)
		{
			if (line != WString::Empty)
			{
				skippedTestCases.Add(line);
			}
		}
	}

	RunTestCasesRpcStdio_Driver(serviceCommand, skippedTestCases);
	ThreadLocalStorage::DisposeStorages();
	FinalizeGlobalStorage();
#if defined VCZH_MSVC && VCZH_CHECK_MEMORY_LEAKS
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}
