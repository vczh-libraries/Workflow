#include "ChatBotJsonDispatcherClient.h"

#include <cwchar>

namespace chatbot
{
	using namespace vl;
	using namespace vl::collections;
	using namespace vl::glr::json;
	using namespace vl::inter_process;
	using namespace vl::reflection;
	using namespace vl::reflection::description;
	using namespace vl::rpc_controller;

	namespace
	{
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

		Ptr<JsonObject> CreateJsonObject()
		{
			return Ptr(new JsonObject);
		}

		Ptr<JsonObject> GetJsonObject(Ptr<JsonNode> node)
		{
			auto object = node.Cast<JsonObject>();
			CHECK_ERROR(object, L"ChatBotJsonDispatcherClient expects a JSON object.");
			return object;
		}

		void AddJsonObjectField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
		{
			auto field = Ptr(new JsonObjectField);
			field->name.value = name;
			field->value = value;
			object->fields.Add(field);
		}

		void SetJsonObjectField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
		{
			for (auto field : object->fields)
			{
				if (field->name.value == name)
				{
					field->value = value;
					return;
				}
			}
			AddJsonObjectField(object, name, value);
		}

		Ptr<JsonNode> FindJsonObjectField(Ptr<JsonObject> object, const WString& name)
		{
			for (auto field : object->fields)
			{
				if (field->name.value == name)
				{
					return field->value;
				}
			}
			return nullptr;
		}

		Ptr<JsonNode> GetJsonObjectField(Ptr<JsonObject> object, const WString& name)
		{
			auto field = FindJsonObjectField(object, name);
			CHECK_ERROR(field, L"ChatBotJsonDispatcherClient expects a JSON object field.");
			return field;
		}

		WString GetJsonString(Ptr<JsonNode> node)
		{
			auto stringNode = node.Cast<JsonString>();
			CHECK_ERROR(stringNode, L"ChatBotJsonDispatcherClient expects a JSON string.");
			return stringNode->content.value;
		}

		vint GetJsonInt(Ptr<JsonNode> node)
		{
			auto numberNode = node.Cast<JsonNumber>();
			CHECK_ERROR(numberNode, L"ChatBotJsonDispatcherClient expects a JSON number.");
			return wtoi(numberNode->content.value);
		}

		bool StartsWith(const WString& value, const wchar_t* prefix)
		{
			auto prefixLength = (vint)wcslen(prefix);
			return value.Length() >= prefixLength && wcsncmp(value.Buffer(), prefix, prefixLength) == 0;
		}

		WString TryReadRpcMethod(Ptr<JsonNode> message)
		{
			if (auto object = message.Cast<JsonObject>())
			{
				if (auto field = FindJsonObjectField(object, WString::Unmanaged(L"rpcMethod")))
				{
					if (auto stringNode = field.Cast<JsonString>())
					{
						return stringNode->content.value;
					}
				}
			}
			return WString::Empty;
		}

		bool IsRpcRequest(const WString& rpcMethod)
		{
			return StartsWith(rpcMethod, L"Request:");
		}

		bool IsRpcResponse(const WString& rpcMethod)
		{
			return StartsWith(rpcMethod, L"Response:");
		}

		bool IsDeclareRemoteServiceRequest(const WString& rpcMethod)
		{
			return rpcMethod == WString::Unmanaged(L"Request:IRpcDispatcher_DeclareRemoteService");
		}

		IRpcJsonMessageDispatcher::RequestType GetRequestType(const WString& rpcMethod)
		{
			if (StartsWith(rpcMethod, L"Request:IObjectOps_"))
			{
				return IRpcJsonMessageDispatcher::RequestType::Direct;
			}
			if (StartsWith(rpcMethod, L"Request:IObjectEventOps_"))
			{
				return IRpcJsonMessageDispatcher::RequestType::Broadcast;
			}
			if (IsDeclareRemoteServiceRequest(rpcMethod))
			{
				return IRpcJsonMessageDispatcher::RequestType::BroadcastAndDrop;
			}
			CHECK_FAIL(L"ChatBotJsonDispatcherClient received an unknown request method.");
			return IRpcJsonMessageDispatcher::RequestType::Direct;
		}

		vint ReadRequestId(Ptr<JsonNode> message)
		{
			return GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"rpcRequestId")));
		}

		vint ReadTargetClientId(Ptr<JsonNode> message)
		{
			return GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"targetClientId")));
		}

		vint ReadDeclaredServiceTypeId(Ptr<JsonNode> message)
		{
			auto ref = GetJsonObject(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"ref")));
			return GetJsonInt(GetJsonObjectField(ref, WString::Unmanaged(L"typeId")));
		}

		Ptr<JsonObject> CreateLoginMessage(vint serverClientId)
		{
			auto message = CreateJsonObject();
			AddJsonObjectField(message, WString::Unmanaged(L"chatBotSystem"), CreateJsonString(WString::Unmanaged(L"Login")));
			AddJsonObjectField(message, WString::Unmanaged(L"serverClientId"), CreateJsonNumber(serverClientId));
			return message;
		}

		bool TryReadLoginMessage(Ptr<JsonNode> message, vint& serverClientId)
		{
			if (auto object = message.Cast<JsonObject>())
			{
				auto systemField = FindJsonObjectField(object, WString::Unmanaged(L"chatBotSystem"));
				if (systemField && GetJsonString(systemField) == WString::Unmanaged(L"Login"))
				{
					serverClientId = GetJsonInt(GetJsonObjectField(object, WString::Unmanaged(L"serverClientId")));
					return true;
				}
			}
			return false;
		}
	}

/***********************************************************************
ChatBotJsonDispatcherClient
***********************************************************************/

	ChatBotJsonDispatcherClient::ChatBotJsonDispatcherClient()
	{
		CHECK_ERROR(semaphoreMessages.Create(0, 65536), L"ChatBotJsonDispatcherClient failed to create the message semaphore.");
		CHECK_ERROR(eventServerLocalClientId.CreateManualUnsignal(false), L"ChatBotJsonDispatcherClient failed to create the server-client-id event.");
	}

	JsonChannel* ChatBotJsonDispatcherClient::GetRpcChannel()
	{
		CHECK_ERROR(rpcChannel, L"RpcChannel has not been initialized.");
		return rpcChannel;
	}

	void ChatBotJsonDispatcherClient::PrepareConnection(JsonChannel* channel, const List<WString>& _waitingForServices)
	{
		CHECK_ERROR(channel, L"ChatBotJsonDispatcherClient needs an RPC channel.");
		CHECK_ERROR(!rpcChannel, L"ChatBotJsonDispatcherClient connection can only be initialized once.");
		rpcChannel = channel;
		CopyFrom(waitingForServices, _waitingForServices);
		if (waitingForServices.Count() > 0)
		{
			eventWaitingForServicesCreated = true;
			CHECK_ERROR(eventWaitingForServices.CreateManualUnsignal(false), L"ChatBotJsonDispatcherClient failed to create the service-waiting event.");
		}
		rpcChannel->Initialize(this);
	}

	void ChatBotJsonDispatcherClient::InitializeRpc(vint clientId)
	{
		auto& app = vl_workflow_global::ChatBotApp::Instance();
		rpcDispatcher = Ptr(new RpcJsonDispatcher(clientId, this));
		lifecycle = Ptr(new RpcJsonLifecycle(clientId, rpcDispatcher.Obj()));

		auto idMap = UnboxParameter<Dictionary<WString, vint>>(BoxParameter(app.rpc_GetIds()));
		lifecycle->SetIdMap(idMap.Ref());

		auto serializer = app.rpcops_IRpcSerializer();
		auto objectOps = app.rpcops_IRpcObjectOpsJson(lifecycle.Obj());
		auto objectEventOps = app.rpcops_IRpcObjectEventOpsJson(lifecycle.Obj());
		ops = app.rpcops_IOps_CreateJson(lifecycle.Obj());

		lifecycle->Register(
			serializer,
			objectOps,
			objectEventOps,
			Func<vint(IDescriptable*)>([](IDescriptable* obj)
			{
				return vl_workflow_global::ChatBotApp::Instance().rpcwrapper_GetTypeId(BoxValue<IDescriptable*>(obj));
			}),
			Func<void(RpcObjectReference, IDescriptable*)>([this](RpcObjectReference ref, IDescriptable* obj)
			{
				vl_workflow_global::ChatBotApp::Instance().rpclistener_Attach(ref.typeId, lifecycle.Obj(), ref, obj, ops);
			})
			);

		lifecycle->RegisterWrapperFactory(Func<Ptr<IRpcWrapperBase>(RpcObjectReference, IRpcLifecycle*)>(
			[this](RpcObjectReference ref, IRpcLifecycle* lc)
			{
				return vl_workflow_global::ChatBotApp::Instance().rpcwrapper_Create(ref, lc, ops);
			}));
	}

	void ChatBotJsonDispatcherClient::ProcessCachedIncomingServiceDeclarations()
	{
		List<JsonPackage> cached;
		SPIN_LOCK(lockServiceDeclarations)
		{
			for (auto request : cachedIncomingServiceDeclarations)
			{
				cached.Add(request);
			}
			cachedIncomingServiceDeclarations.Clear();
		}

		for (auto request : cached)
		{
			ProcessIncomingServiceDeclaration(request);
		}
	}

	void ChatBotJsonDispatcherClient::SendCachedOutgoingServiceDeclarations()
	{
		List<JsonPackage> cached;
		SPIN_LOCK(lockServiceDeclarations)
		{
			for (auto request : cachedOutgoingServiceDeclarations)
			{
				cached.Add(request);
			}
			cachedOutgoingServiceDeclarations.Clear();
		}

		for (auto request : cached)
		{
			SendJsonRequest(request, RequestType::BroadcastAndDrop);
		}
	}

	void ChatBotJsonDispatcherClient::ProcessIncomingServiceDeclaration(JsonPackage request)
	{
		IRpcJsonMessageDispatcher::DefaultTranslate(
			request,
			RequestType::BroadcastAndDrop,
			lifecycle->GetController()->GetObjectOps(),
			lifecycle->GetController()->GetObjectEventOps(),
			lifecycle->GetDispatcher(),
			lifecycle.Obj()
			);
		UpdateWaitingForServices(request);
	}

	void ChatBotJsonDispatcherClient::UpdateWaitingForServices(JsonPackage request)
	{
		auto typeId = ReadDeclaredServiceTypeId(request);
		bool shouldSignal = false;
		SPIN_LOCK(lockWaitingForServices)
		{
			for (vint i = waitingForServices.Count(); i > 0; i--)
			{
				auto index = i - 1;
				if (lifecycle->GetTypeIdFromName(waitingForServices[index]) == typeId)
				{
					waitingForServices.RemoveAt(index);
				}
			}
			shouldSignal = waitingForServices.Count() == 0;
		}
		if (shouldSignal && eventWaitingForServicesCreated)
		{
			eventWaitingForServices.Signal();
		}
	}

	void ChatBotJsonDispatcherClient::WaitForServerClientId()
	{
		if (serverLocalClientId.load() == -1)
		{
			eventServerLocalClientId.Wait();
		}
	}

	void ChatBotJsonDispatcherClient::WaitForExpectedServices()
	{
		if (eventWaitingForServicesCreated)
		{
			eventWaitingForServices.Wait();
		}
	}

	void ChatBotJsonDispatcherClient::SendJsonRequest(JsonPackage message, RequestType requestType)
	{
		switch (requestType)
		{
		case RequestType::Direct:
			GetRpcChannel()->SendToClient(ReadTargetClientId(message), message);
			break;
		case RequestType::Broadcast:
		case RequestType::BroadcastAndDrop:
			WaitForServerClientId();
			GetRpcChannel()->SendToClient(serverLocalClientId.load(), message);
			break;
		default:
			CHECK_FAIL(L"ChatBotJsonDispatcherClient received an unknown request type.");
		}
		FlushChannel();
	}

	void ChatBotJsonDispatcherClient::PushReceivedMessage(vint senderClientId, JsonPackage message)
	{
		SPIN_LOCK(lockMessages)
		{
			messages.Add({ senderClientId, message });
		}
		semaphoreMessages.Release();
	}

	ChatBotJsonDispatcherClient::ReceivedJsonMessage ChatBotJsonDispatcherClient::PopReceivedMessage()
	{
		semaphoreMessages.Wait();
		ReceivedJsonMessage result;
		SPIN_LOCK(lockMessages)
		{
			CHECK_ERROR(messages.Count() > 0, L"ChatBotJsonDispatcherClient message queue is unexpectedly empty.");
			result = messages[0];
			messages.RemoveAt(0);
		}
		return result;
	}

	bool ChatBotJsonDispatcherClient::TryPopBufferedResponse(vint requestId, ReceivedJsonMessage& message)
	{
		SPIN_LOCK(lockMessages)
		{
			for (vint i = 0; i < bufferedResponses.Count(); i++)
			{
				auto item = bufferedResponses[i];
				if (ReadRequestId(item.message) == requestId)
				{
					message = item;
					bufferedResponses.RemoveAt(i);
					return true;
				}
			}
		}
		return false;
	}

	void ChatBotJsonDispatcherClient::PushBufferedResponse(ReceivedJsonMessage message)
	{
		SPIN_LOCK(lockMessages)
		{
			bufferedResponses.Add(message);
		}
	}

	void ChatBotJsonDispatcherClient::SendJsonResponse(vint receiverClientId, JsonPackage response)
	{
		if (response)
		{
			GetRpcChannel()->SendToClient(receiverClientId, response);
			FlushChannel();
		}
	}

	void ChatBotJsonDispatcherClient::FlushChannel()
	{
		bool disconnected = false;
		GetRpcChannel()->BatchWrite(disconnected);
	}

	void ChatBotJsonDispatcherClient::ProcessRequestAndSendResponse(vint senderClientId, JsonPackage request)
	{
		auto response = TranslateRequest(request);
		SendJsonResponse(senderClientId, response);
	}

	JsonPackage ChatBotJsonDispatcherClient::TranslateRequest(JsonPackage request)
	{
		auto rpcMethod = TryReadRpcMethod(request);
		CHECK_ERROR(IsRpcRequest(rpcMethod), L"ChatBotJsonDispatcherClient expects an RPC request.");

		return IRpcJsonMessageDispatcher::DefaultTranslate(
			request,
			GetRequestType(rpcMethod),
			lifecycle->GetController()->GetObjectOps(),
			lifecycle->GetController()->GetObjectEventOps(),
			lifecycle->GetDispatcher(),
			lifecycle.Obj()
			);
	}

	void ChatBotJsonDispatcherClient::WaitForServer(JsonChannelClient* channelClient, JsonChannel* channel, const List<WString>& _waitingForServices)
	{
		CHECK_ERROR(channelClient, L"ChatBotJsonDispatcherClient needs a channel client.");
		PrepareConnection(channel, _waitingForServices);
		channelClient->WaitForServer();
		CHECK_ERROR(channelClient->GetStatus() == ClientStatus::Connected, L"ChatBotJsonDispatcherClient failed to connect to the server.");
		InitializeRpc(channelClient->GetClientId());
		Initialize();
	}

	vint ChatBotJsonDispatcherClient::ConnectLocalClient(JsonChannelServer* channelServer, Ptr<JsonChannelClient> localClient, JsonChannel* channel, const List<WString>& _waitingForServices)
	{
		CHECK_ERROR(channelServer, L"ChatBotJsonDispatcherClient needs a channel server.");
		CHECK_ERROR(localClient, L"ChatBotJsonDispatcherClient needs a local client.");
		PrepareConnection(channel, _waitingForServices);
		auto clientId = channelServer->ConnectLocalClient(localClient);
		CHECK_ERROR(clientId != -1, L"ChatBotJsonDispatcherClient failed to connect the local client.");
		InitializeRpc(clientId);
		return clientId;
	}

	void ChatBotJsonDispatcherClient::RegisterLocalService(Ptr<chatapi::IChatServer> service)
	{
		CHECK_ERROR(lifecycle, L"ChatBotJsonDispatcherClient needs to connect before registering services.");
		CHECK_ERROR(initialized.load() == 0, L"ChatBotJsonDispatcherClient services must be registered before Initialize.");
		CHECK_ERROR(service, L"ChatBotJsonDispatcherClient needs an IChatServer service.");
		auto typeId = lifecycle->GetTypeIdFromName(WString::Unmanaged(L"chatapi::IChatServer"));
		CHECK_ERROR(typeId != RpcTypeId_NotFound, L"ChatBotJsonDispatcherClient failed to find IChatServer type id.");
		lifecycle->RegisterLocalService(typeId, service);
	}

	void ChatBotJsonDispatcherClient::Initialize()
	{
		CHECK_ERROR(lifecycle, L"ChatBotJsonDispatcherClient needs to connect before Initialize.");
		CHECK_ERROR(initialized.load() == 0, L"ChatBotJsonDispatcherClient can only be initialized once.");
		ProcessCachedIncomingServiceDeclarations();
		lifecycle->Initialize();
		initialized.store(1);
		SendCachedOutgoingServiceDeclarations();
		WaitForExpectedServices();

		chatServer = lifecycle->RequestService(WString::Unmanaged(L"chatapi::IChatServer")).Cast<chatapi::IChatServer>();
	}

	Ptr<chatapi::IChatServer> ChatBotJsonDispatcherClient::GetChatServer()
	{
		CHECK_ERROR(chatServer, L"ChatBotJsonDispatcherClient failed to request IChatServer.");
		return chatServer;
	}

	vint ChatBotJsonDispatcherClient::AllocateRequestId()
	{
		return ++nextRequestId;
	}

	JsonPackage ChatBotJsonDispatcherClient::OnJsonRequest(JsonPackage message, RequestType requestType)
	{
		auto rpcMethod = TryReadRpcMethod(message);
		CHECK_ERROR(rpcMethod != WString::Empty, L"ChatBotJsonDispatcherClient expects an RPC message.");

		if (requestType == RequestType::BroadcastAndDrop)
		{
			CHECK_ERROR(IsDeclareRemoteServiceRequest(rpcMethod), L"ChatBotJsonDispatcherClient expects a service declaration.");
			if (initialized.load() == 0)
			{
				SPIN_LOCK(lockServiceDeclarations)
				{
					cachedOutgoingServiceDeclarations.Add(message);
				}
			}
			else
			{
				SendJsonRequest(message, requestType);
			}
			return nullptr;
		}

		CHECK_ERROR(initialized.load() != 0, L"ChatBotJsonDispatcherClient only accepts service declarations before Initialize.");

		struct ActiveJsonRequest
		{
			atomic_vint& counter;

			ActiveJsonRequest(atomic_vint& _counter)
				: counter(_counter)
			{
				counter++;
			}

			~ActiveJsonRequest()
			{
				counter--;
			}
		};

		ActiveJsonRequest active(activeJsonRequests);
		auto requestId = ReadRequestId(message);
		SendJsonRequest(message, requestType);

		while (true)
		{
			ReceivedJsonMessage item;
			if (!TryPopBufferedResponse(requestId, item))
			{
				item = PopReceivedMessage();
			}
			rpcMethod = TryReadRpcMethod(item.message);
			CHECK_ERROR(rpcMethod != WString::Empty, L"ChatBotJsonDispatcherClient expects an RPC message.");

			if (IsRpcRequest(rpcMethod))
			{
				ProcessRequestAndSendResponse(item.senderClientId, item.message);
				continue;
			}

			CHECK_ERROR(IsRpcResponse(rpcMethod), L"ChatBotJsonDispatcherClient expects an RPC request or response.");
			if (ReadRequestId(item.message) == requestId)
			{
				return item.message;
			}

			PushBufferedResponse(item);
		}
	}

	void ChatBotJsonDispatcherClient::OnRead(vint senderClientId, const JsonPackage& package)
	{
		vint newServerLocalClientId = -1;
		if (TryReadLoginMessage(package, newServerLocalClientId))
		{
			serverLocalClientId.store(newServerLocalClientId);
			eventServerLocalClientId.Signal();
			return;
		}

		auto rpcMethod = TryReadRpcMethod(package);
		CHECK_ERROR(rpcMethod != WString::Empty, L"ChatBotJsonDispatcherClient expects an RPC message.");

		if (IsDeclareRemoteServiceRequest(rpcMethod))
		{
			if (initialized.load() == 0)
			{
				SPIN_LOCK(lockServiceDeclarations)
				{
					cachedIncomingServiceDeclarations.Add(package);
				}
			}
			else
			{
				ProcessIncomingServiceDeclaration(package);
			}
			return;
		}

		CHECK_ERROR(initialized.load() != 0, L"ChatBotJsonDispatcherClient only accepts service declarations before Initialize.");

		if (activeJsonRequests.load() > 0)
		{
			PushReceivedMessage(senderClientId, package);
			return;
		}

		if (IsRpcRequest(rpcMethod))
		{
			SchedulaTask(Func<void()>([this, senderClientId, package]()
			{
				ProcessRequestAndSendResponse(senderClientId, package);
			}));
		}
		else
		{
			CHECK_ERROR(IsRpcResponse(rpcMethod), L"ChatBotJsonDispatcherClient expects an RPC request or response.");
			CHECK_FAIL(L"ChatBotJsonDispatcherClient received an RPC response while no request is waiting.");
		}
	}

	void ChatBotJsonDispatcherClient::FinalizeRpc()
	{
		if (lifecycle)
		{
			lifecycle->Finalize();
			lifecycle = nullptr;
			rpcDispatcher = nullptr;
			ops = nullptr;
			chatServer = nullptr;
		}
	}
}
