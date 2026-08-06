#include "../../Source/RpcDualJsonDispatcherMock.h"
#include "../../../Source/Library/RpcJson/WfLibraryRpcJsonDispatcherClient.h"

using namespace vl;
using namespace vl::collections;
using namespace vl::glr::json;
using namespace vl::inter_process;
using namespace vl::reflection::description;
using namespace vl::rpc_controller;
using namespace vl::rpc_controller::channeling;
using namespace vl::rpc_controller_test;

WString GetJsonValuesOutputPath()
{
	return WString::Empty;
}

WString GetJsonRequestOutputPath()
{
	return WString::Empty;
}

namespace vl::workflow_library_test
{
	void AddJsonField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
	{
		auto field = Ptr(new JsonObjectField);
		field->name.value = name;
		field->value = value;
		object->fields.Add(field);
	}

	Ptr<JsonString> CreateJsonString(const WString& value)
	{
		auto node = Ptr(new JsonString);
		node->content.value = value;
		return node;
	}

	Ptr<JsonNumber> CreateJsonNumber(vint value)
	{
		auto node = Ptr(new JsonNumber);
		node->content.value = itow(value);
		return node;
	}

	Ptr<JsonObject> CreateRpcMessage(const WString& rpcMethod, vint requestId, vint sourceClientId, vint targetClientId)
	{
		auto message = Ptr(new JsonObject);
		AddJsonField(message, WString::Unmanaged(L"rpcMethod"), CreateJsonString(rpcMethod));
		AddJsonField(message, WString::Unmanaged(L"rpcRequestId"), CreateJsonNumber(requestId));
		AddJsonField(message, WString::Unmanaged(L"sourceClientId"), CreateJsonNumber(sourceClientId));
		AddJsonField(message, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(targetClientId));
		return message;
	}

	Ptr<JsonObject> CreateDirectRequest(vint requestId)
	{
		return CreateRpcMessage(WString::Unmanaged(L"Request:IObjectOps_EndInvokeMethod"), requestId, 1, 2);
	}

	Ptr<JsonObject> CreateDirectResponse(vint requestId)
	{
		return CreateRpcMessage(WString::Unmanaged(L"Response:IObjectOps_EndInvokeMethod"), requestId, 2, 1);
	}

	Ptr<JsonObject> CreateNestedRequest(vint requestId)
	{
		auto message = CreateRpcMessage(WString::Unmanaged(L"Request:IObjectOps_EndInvokeMethod"), requestId, 2, 1);
		AddJsonField(message, WString::Unmanaged(L"slot"), CreateJsonNumber(0));
		return message;
	}

	WString ReadRpcMethod(Ptr<JsonNode> message)
	{
		auto object = message.Cast<JsonObject>();
		CHECK_ERROR(object, L"ReadRpcMethod(...)#A JSON object is required.");
		for (auto field : object->fields)
		{
			if (field->name.value == WString::Unmanaged(L"rpcMethod"))
			{
				auto value = field->value.Cast<JsonString>();
				CHECK_ERROR(value, L"ReadRpcMethod(...)#The rpcMethod field must be a string.");
				return value->content.value;
			}
		}
		CHECK_FAIL(L"ReadRpcMethod(...)#The rpcMethod field is missing.");
		return WString::Empty;
	}

	class TestJsonChannel : public Object, public JsonChannel
	{
	private:
		WString						channelName = WString::Unmanaged(L"Rpc");
		IChannelReader<JsonPackage>*	reader = nullptr;
		SpinLock						lockState;
		vint							sentCount = 0;
		Func<void(JsonPackage)>			onSend;
		EventObject						sent;

	public:
		TestJsonChannel()
		{
			CHECK_ERROR(sent.CreateAutoUnsignal(false), L"TestJsonChannel::TestJsonChannel()#Failed to create the sent event.");
		}

		const WString& GetChannelName() override
		{
			return channelName;
		}

		IChannelReader<JsonPackage>* GetReader() override
		{
			return reader;
		}

		void Initialize(IChannelReader<JsonPackage>* receiver) override
		{
			CHECK_ERROR(receiver && !reader, L"TestJsonChannel::Initialize(...)#A reader can only be installed once.");
			reader = receiver;
		}

		void SendToClient(vint, const JsonPackage& package) override
		{
			Func<void(JsonPackage)> callback;
			SPIN_LOCK(lockState)
			{
				sentCount++;
				callback = onSend;
			}
			sent.Signal();
			if (callback)
			{
				callback(package);
			}
		}

		void BroadcastFromClient(const JsonPackage&) override
		{
			CHECK_FAIL(L"Not Supported!");
		}

		void BroadcastFromClient(const JsonPackage&, const List<vint>&) override
		{
			CHECK_FAIL(L"Not Supported!");
		}

		void BatchWrite(bool& disconnected) override
		{
			disconnected = false;
		}

		bool WaitForSent()
		{
			return sent.WaitForTime(5000);
		}

		vint GetSentCount()
		{
			vint result = 0;
			SPIN_LOCK(lockState)
			{
				result = sentCount;
			}
			return result;
		}

		void SetOnSend(const Func<void(JsonPackage)>& callback)
		{
			SPIN_LOCK(lockState)
			{
				onSend = callback;
			}
		}

		void Deliver(vint senderClientId, JsonPackage package)
		{
			CHECK_ERROR(reader, L"TestJsonChannel::Deliver(...)#The reader is missing.");
			reader->OnRead(senderClientId, package);
		}
	};

	class TestJsonChannelClient : public Object, public JsonChannelClient
	{
	private:
		ChannelMap channels;

	public:
		void OnConnected(vint) override
		{
		}

		void OnDisconnected() override
		{
		}

		void OnReadError(const WString&) override
		{
		}

		void OnLocalError(const WString&, bool) override
		{
		}

		const ChannelNameList& OnGetChannelNames() override
		{
			return channels.Keys();
		}

		const ChannelMap& GetChannels() override
		{
			return channels;
		}

		vint GetClientId() override
		{
			return 1;
		}

		void WaitForServer() override
		{
		}

		ClientStatus GetStatus() override
		{
			return ClientStatus::Connected;
		}

		void BroadcastError(const WString&) override
		{
			CHECK_FAIL(L"Not Supported!");
		}
	};

	class TestObjectOps : public Object, public IRpcObjectOps
	{
	public:
		Value InvokeMethod(RpcObjectReference, vint, Ptr<IValueArray>) override
		{
			CHECK_FAIL(L"Not Supported!");
			return {};
		}

		void EndInvokeMethod(vint) override
		{
		}

		void ObjectHold(RpcObjectReference, vint, bool) override
		{
			CHECK_FAIL(L"Not Supported!");
		}
	};

	class TestObjectEventOps : public Object, public IRpcObjectEventOps
	{
	public:
		Value InvokeEvent(RpcObjectReference, vint, Ptr<IValueArray>) override
		{
			CHECK_FAIL(L"Not Supported!");
			return {};
		}
	};

	class TestRpcJsonDispatcherClient : public RpcJsonDispatcherClient
	{
	private:
		Ptr<RpcJsonLifecycle> lifecycle;
		Ptr<IDescriptable> localService;

	protected:
		void ScheduleTask(Func<void()> task) override
		{
			task();
		}

	public:
		void InitializeRpc(vint clientId)
		{
			auto dispatcher = Ptr(new RpcJsonDispatcher(clientId, this));
			lifecycle = Ptr(new RpcJsonLifecycle(clientId, dispatcher.Obj()));
			SetRpcObjects(dispatcher, lifecycle);
			lifecycle->Register(
				nullptr,
				Ptr(new TestObjectOps),
				Ptr(new TestObjectEventOps),
				{},
				{}
				);
		}

		void RegisterLocalServiceForStartupTest()
		{
			Dictionary<WString, vint> idMap;
			idMap.Add(WString::Unmanaged(L"system::IValueList"), RpcTypeId_IValueList);
			lifecycle->SetIdMap(idMap);
			localService = IValueList::Create();
			lifecycle->RegisterLocalService(RpcTypeId_IValueList, localService);
		}
	};

	class RequestThread : public Thread
	{
	private:
		IRpcJsonMessageDispatcher* dispatcher;
		JsonPackage request;

	protected:
		void Run() override
		{
			callerThreadId = Thread::GetCurrentThreadId();
			try
			{
				response = dispatcher->OnJsonRequest(request, IRpcJsonMessageDispatcher::RequestType::Direct);
				returned = true;
			}
			catch (const RpcInjectedException& ex)
			{
				catchThreadId = Thread::GetCurrentThreadId();
				error = ex.Message();
			}
		}

	public:
		bool returned = false;
		vint callerThreadId = -1;
		vint catchThreadId = -1;
		WString error;
		JsonPackage response;

		RequestThread(IRpcJsonMessageDispatcher* _dispatcher, JsonPackage _request)
			: dispatcher(_dispatcher)
			, request(_request)
		{
		}
	};

	class InitializeThread : public Thread
	{
	private:
		RpcJsonDispatcherClient* dispatcher;

	protected:
		void Run() override
		{
			try
			{
				dispatcher->Initialize();
				returned = true;
			}
			catch (const RpcInjectedException& ex)
			{
				error = ex.Message();
			}
		}

	public:
		bool returned = false;
		WString error;

		InitializeThread(RpcJsonDispatcherClient* _dispatcher)
			: dispatcher(_dispatcher)
		{
		}
	};

	void AssertInjected(IRpcJsonMessageDispatcher& dispatcher, JsonPackage request, IRpcJsonMessageDispatcher::RequestType requestType, const WString& expected)
	{
		bool thrown = false;
		try
		{
			dispatcher.OnJsonRequest(request, requestType);
		}
		catch (const RpcInjectedException& ex)
		{
			thrown = true;
			TEST_ASSERT(ex.Message() == expected);
		}
		TEST_ASSERT(thrown);
	}
}

using namespace vl::workflow_library_test;

TEST_FILE
{
	TEST_CASE(L"RpcJson direct bridge keeps a persistent last-write-wins injected exception")
	{
		RpcDualJsonRequestBridge bridge;
		auto request = Ptr(new JsonObject);

		bridge.InjectException(L"A");
		bridge.InjectException(L"B");
		AssertInjected(bridge, request, IRpcJsonMessageDispatcher::RequestType::Direct, L"B");
		AssertInjected(bridge, request, IRpcJsonMessageDispatcher::RequestType::BroadcastAndDrop, L"B");

		bridge.InjectException(L"");
		AssertInjected(bridge, request, IRpcJsonMessageDispatcher::RequestType::Direct, L"");
	});

	TEST_CASE(L"RpcJson channel injection is persistent and prevents every later send")
	{
		TestRpcJsonDispatcherClient dispatcher;
		dispatcher.InjectException(L"A");
		dispatcher.InjectException(L"B");
		auto request = CreateDirectRequest(1);
		AssertInjected(dispatcher, request, IRpcJsonMessageDispatcher::RequestType::Direct, L"B");
		AssertInjected(dispatcher, request, IRpcJsonMessageDispatcher::RequestType::BroadcastAndDrop, L"B");

		dispatcher.InjectException(L"");
		AssertInjected(dispatcher, request, IRpcJsonMessageDispatcher::RequestType::Direct, L"");
	});

	TEST_CASE(L"RpcJson channel injection wakes an in-flight request on its caller thread")
	{
		TestJsonChannel channel;
		TestJsonChannelClient channelClient;
		TestRpcJsonDispatcherClient dispatcher;
		List<WString> waitingForServices;
		dispatcher.WaitForServer(&channelClient, &channel, waitingForServices);
		dispatcher.InitializeRpc(1);
		dispatcher.Initialize();

		RequestThread requestThread(&dispatcher, CreateDirectRequest(1));
		TEST_ASSERT(requestThread.Start());
		TEST_ASSERT(channel.WaitForSent());
		dispatcher.InjectException(L"in-flight");
		TEST_ASSERT(requestThread.WaitForTime(5000));
		requestThread.Wait();
		TEST_ASSERT(!requestThread.returned);
		TEST_ASSERT(requestThread.error == L"in-flight");
		TEST_ASSERT(requestThread.callerThreadId == requestThread.catchThreadId);
		TEST_ASSERT(channel.GetSentCount() == 1);
	});

	TEST_CASE(L"RpcJson response commitment can win before persistent injection")
	{
		TestJsonChannel channel;
		TestJsonChannelClient channelClient;
		TestRpcJsonDispatcherClient dispatcher;
		List<WString> waitingForServices;
		dispatcher.WaitForServer(&channelClient, &channel, waitingForServices);
		dispatcher.InitializeRpc(1);
		dispatcher.Initialize();

		RequestThread requestThread(&dispatcher, CreateDirectRequest(1));
		TEST_ASSERT(requestThread.Start());
		TEST_ASSERT(channel.WaitForSent());
		channel.Deliver(2, CreateDirectResponse(1));
		TEST_ASSERT(requestThread.WaitForTime(5000));
		requestThread.Wait();
		TEST_ASSERT(requestThread.returned);
		TEST_ASSERT(requestThread.response);

		dispatcher.InjectException(L"after-response");
		AssertInjected(dispatcher, CreateDirectRequest(2), IRpcJsonMessageDispatcher::RequestType::Direct, L"after-response");
	});

	TEST_CASE(L"RpcJson nested request processing rechecks persistent injection")
	{
		TestJsonChannel channel;
		TestJsonChannelClient channelClient;
		TestRpcJsonDispatcherClient dispatcher;
		List<WString> waitingForServices;
		dispatcher.WaitForServer(&channelClient, &channel, waitingForServices);
		dispatcher.InitializeRpc(1);
		dispatcher.Initialize();

		RequestThread requestThread(&dispatcher, CreateDirectRequest(1));
		TEST_ASSERT(requestThread.Start());
		TEST_ASSERT(channel.WaitForSent());
		channel.SetOnSend(Func<void(JsonPackage)>([&](JsonPackage package)
		{
			if (ReadRpcMethod(package) == WString::Unmanaged(L"Response:IObjectOps_EndInvokeMethod"))
			{
				dispatcher.InjectException(L"nested");
			}
		}));
		channel.Deliver(2, CreateNestedRequest(99));
		TEST_ASSERT(requestThread.WaitForTime(5000));
		requestThread.Wait();
		TEST_ASSERT(!requestThread.returned);
		TEST_ASSERT(requestThread.error == L"nested");
	});

	TEST_CASE(L"RpcJson injection releases required-service startup")
	{
		TestJsonChannel channel;
		TestJsonChannelClient channelClient;
		TestRpcJsonDispatcherClient dispatcher;
		List<WString> waitingForServices;
		waitingForServices.Add(WString::Unmanaged(L"missing::IService"));
		dispatcher.WaitForServer(&channelClient, &channel, waitingForServices);
		dispatcher.InitializeRpc(1);
		dispatcher.SetServerLocalClientId(0);
		dispatcher.RegisterLocalServiceForStartupTest();

		InitializeThread initializeThread(&dispatcher);
		TEST_ASSERT(initializeThread.Start());
		TEST_ASSERT(channel.WaitForSent());
		dispatcher.InjectException(L"startup");
		TEST_ASSERT(initializeThread.WaitForTime(5000));
		initializeThread.Wait();
		TEST_ASSERT(!initializeThread.returned);
		TEST_ASSERT(initializeThread.error == L"startup");
		dispatcher.FinalizeRpc();
	});
}
