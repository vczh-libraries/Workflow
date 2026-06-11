#include "ChatBotJsonDispatcher.h"

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
		Ptr<JsonLiteral> CreateJsonLiteral(JsonLiteralValue value)
		{
			auto node = Ptr(new JsonLiteral);
			node->value = value;
			return node;
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

		Ptr<JsonArray> CreateJsonArray()
		{
			return Ptr(new JsonArray);
		}

		Ptr<JsonObject> CreateJsonObject()
		{
			return Ptr(new JsonObject);
		}

		Ptr<JsonObject> GetJsonObject(Ptr<JsonNode> node)
		{
			auto object = node.Cast<JsonObject>();
			CHECK_ERROR(object, L"ChatBotJsonDispatcher expects a JSON object.");
			return object;
		}

		Ptr<JsonArray> GetJsonArray(Ptr<JsonNode> node)
		{
			auto array = node.Cast<JsonArray>();
			CHECK_ERROR(array, L"ChatBotJsonDispatcher expects a JSON array.");
			return array;
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
			CHECK_ERROR(field, L"ChatBotJsonDispatcher expects a JSON object field.");
			return field;
		}

		WString GetJsonString(Ptr<JsonNode> node)
		{
			auto stringNode = node.Cast<JsonString>();
			CHECK_ERROR(stringNode, L"ChatBotJsonDispatcher expects a JSON string.");
			return stringNode->content.value;
		}

		vint GetJsonInt(Ptr<JsonNode> node)
		{
			auto numberNode = node.Cast<JsonNumber>();
			CHECK_ERROR(numberNode, L"ChatBotJsonDispatcher expects a JSON number.");
			return wtoi(numberNode->content.value);
		}

		bool IsJsonNull(Ptr<JsonNode> node)
		{
			auto literal = node.Cast<JsonLiteral>();
			return literal && literal->value == JsonLiteralValue::Null;
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

		bool IsBroadcastRequest(const WString& rpcMethod)
		{
			return
				rpcMethod == WString::Unmanaged(L"Request:IObjectEventOps_InvokeEvent") ||
				rpcMethod == WString::Unmanaged(L"Request:IRpcDispatcher_DeclareLocalService");
		}

		bool IsRemoveUserRequest(Ptr<JsonNode> message)
		{
			if (TryReadRpcMethod(message) != WString::Unmanaged(L"Request:IObjectOps_InvokeMethod"))
			{
				return false;
			}

			auto methodId = GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"methodId")));
			return methodId == vl_workflow_global::ChatBotApp::Instance().rpcmethod_chatapi__IChatServer_RemoveUser;
		}

		vint ReadSourceClientId(Ptr<JsonNode> message)
		{
			return GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"sourceClientId")));
		}

		vint ReadTargetClientId(Ptr<JsonNode> message)
		{
			return GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"targetClientId")));
		}

		void WriteSourceClientId(Ptr<JsonNode> message, vint clientId)
		{
			SetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"sourceClientId"), CreateJsonNumber(clientId));
		}

		Ptr<JsonObject> CreateRpcMessage(const WString& rpcMethod, vint requestId, vint sourceClientId)
		{
			auto message = CreateJsonObject();
			AddJsonObjectField(message, WString::Unmanaged(L"rpcMethod"), CreateJsonString(rpcMethod));
			AddJsonObjectField(message, WString::Unmanaged(L"rpcRequestId"), CreateJsonNumber(requestId));
			AddJsonObjectField(message, WString::Unmanaged(L"sourceClientId"), CreateJsonNumber(sourceClientId));
			return message;
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

		bool IsBroadcastReady(Ptr<ChatBotJsonDispatcherServer::PendingBroadcast> pending)
		{
			for (auto clientId : pending->expectedClientIds)
			{
				if (!pending->responses.Keys().Contains(clientId))
				{
					return false;
				}
			}
			return true;
		}
	}

/***********************************************************************
TaskQueue
***********************************************************************/

	Ptr<Parser> CreateChatBotJsonParser()
	{
		return Ptr(new Parser);
	}

	TaskQueue::TaskQueue()
	{
		CHECK_ERROR(semaphoreTasks.Create(0, 65536), L"TaskQueue failed to create the task semaphore.");
	}

	void TaskQueue::QueueTask(Func<void()> task)
	{
		SPIN_LOCK(lockTasks)
		{
			tasks.Add(task);
		}
		semaphoreTasks.Release();
	}

	void TaskQueue::QueueExitTask()
	{
		SPIN_LOCK(lockTasks)
		{
			exitTaskQueued = true;
		}
		semaphoreTasks.Release();
	}

	void TaskQueue::RunTaskQueue()
	{
		while (true)
		{
			Func<void()> task;
			bool hasTask = false;
			bool shouldExit = false;
			SPIN_LOCK(lockTasks)
			{
				if (tasks.Count() > 0)
				{
					task = tasks[0];
					tasks.RemoveAt(0);
					hasTask = true;
				}
				else
				{
					shouldExit = exitTaskQueued;
				}
			}

			if (shouldExit)
			{
				break;
			}
			if (!hasTask)
			{
				semaphoreTasks.Wait();
				continue;
			}
			task();
		}
	}

/***********************************************************************
ChatBotJsonDispatcherBase
***********************************************************************/

	ChatBotJsonDispatcherBase::ChatBotJsonDispatcherBase(JsonChannel* channel)
	{
		CHECK_ERROR(semaphoreMessages.Create(0, 65536), L"ChatBotJsonDispatcherBase failed to create the message semaphore.");
		CHECK_ERROR(channel, L"ChatBotJsonDispatcherBase needs an RPC channel.");
		rpcChannel = channel;
		rpcChannel->Initialize(this);
	}

	vint ChatBotJsonDispatcherBase::GetChatServerTypeId()
	{
		return vl_workflow_global::ChatBotApp::Instance().rpctype_chatapi__IChatServer;
	}

	JsonChannel* ChatBotJsonDispatcherBase::GetRpcChannel()
	{
		CHECK_ERROR(rpcChannel, L"RpcChannel has not been initialized.");
		return rpcChannel;
	}

	vint ChatBotJsonDispatcherBase::GetLocalClientId()
	{
		CHECK_ERROR(lifecycle, L"RPC lifecycle has not been initialized.");
		return lifecycle->GetClientId();
	}

	void ChatBotJsonDispatcherBase::InitializeRpc(vint clientId)
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

	void ChatBotJsonDispatcherBase::PushReceivedMessage(vint senderClientId, JsonPackage message)
	{
		SPIN_LOCK(lockMessages)
		{
			messages.Add({ senderClientId, message });
		}
		semaphoreMessages.Release();
	}

	ChatBotJsonDispatcherBase::ReceivedJsonMessage ChatBotJsonDispatcherBase::PopReceivedMessage()
	{
		semaphoreMessages.Wait();
		ReceivedJsonMessage result;
		SPIN_LOCK(lockMessages)
		{
			CHECK_ERROR(messages.Count() > 0, L"ChatBotJsonDispatcherBase message queue is unexpectedly empty.");
			result = messages[0];
			messages.RemoveAt(0);
		}
		return result;
	}

	bool ChatBotJsonDispatcherBase::TryPopBufferedResponse(vint requestId, ReceivedJsonMessage& message)
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

	void ChatBotJsonDispatcherBase::PushBufferedResponse(ReceivedJsonMessage message)
	{
		SPIN_LOCK(lockMessages)
		{
			bufferedResponses.Add(message);
		}
	}

	void ChatBotJsonDispatcherBase::SendJsonResponse(vint receiverClientId, JsonPackage response)
	{
		GetRpcChannel()->SendToClient(receiverClientId, response);
		FlushChannel();
	}

	void ChatBotJsonDispatcherBase::FlushChannel()
	{
		bool disconnected = false;
		GetRpcChannel()->BatchWrite(disconnected);
	}

	bool ChatBotJsonDispatcherBase::HandleIncomingRequest(vint, JsonPackage)
	{
		return false;
	}

	bool ChatBotJsonDispatcherBase::HandleUnmatchedResponse(vint, JsonPackage)
	{
		return false;
	}

	void ChatBotJsonDispatcherBase::AfterRequestResponse(vint, JsonPackage)
	{
	}

	void ChatBotJsonDispatcherBase::ProcessRequestAndSendResponse(vint senderClientId, JsonPackage request)
	{
		if (HandleIncomingRequest(senderClientId, request))
		{
			return;
		}
		auto response = TranslateRequest(request);
		SendJsonResponse(senderClientId, response);
		AfterRequestResponse(senderClientId, request);
	}

	JsonPackage ChatBotJsonDispatcherBase::TranslateRequest(JsonPackage request)
	{
		auto rpcMethod = TryReadRpcMethod(request);
		CHECK_ERROR(IsRpcRequest(rpcMethod), L"ChatBotJsonDispatcherBase expects an RPC request.");

		return IRpcJsonMessageDispatcher::DefaultDispatch(
			request,
			IsBroadcastRequest(rpcMethod),
			lifecycle->GetController()->GetObjectOps(),
			lifecycle->GetController()->GetObjectEventOps(),
			lifecycle->GetDispatcher(),
			lifecycle.Obj()
			);
	}

	vint ChatBotJsonDispatcherBase::AllocateRequestId()
	{
		return ++nextRequestId;
	}

	JsonPackage ChatBotJsonDispatcherBase::OnJsonRequest(JsonPackage message, bool broadcast)
	{
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
		SendJsonRequest(message, broadcast);

		while (true)
		{
			ReceivedJsonMessage item;
			if (!TryPopBufferedResponse(requestId, item))
			{
				item = PopReceivedMessage();
			}
			auto rpcMethod = TryReadRpcMethod(item.message);
			CHECK_ERROR(rpcMethod != WString::Empty, L"ChatBotJsonDispatcherBase expects an RPC message.");

			if (IsRpcRequest(rpcMethod))
			{
				ProcessRequestAndSendResponse(item.senderClientId, item.message);
				continue;
			}

			CHECK_ERROR(IsRpcResponse(rpcMethod), L"ChatBotJsonDispatcherBase expects an RPC request or response.");
			if (ReadRequestId(item.message) == requestId)
			{
				return item.message;
			}

			if (!HandleUnmatchedResponse(item.senderClientId, item.message))
			{
				PushBufferedResponse(item);
			}
		}
	}

	void ChatBotJsonDispatcherBase::OnRead(vint senderClientId, const JsonPackage& package)
	{
		vint ignoredServerClientId = -1;
		if (TryReadLoginMessage(package, ignoredServerClientId))
		{
			PushReceivedMessage(senderClientId, package);
			return;
		}

		auto rpcMethod = TryReadRpcMethod(package);
		CHECK_ERROR(rpcMethod != WString::Empty, L"ChatBotJsonDispatcherBase expects an RPC message.");

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
			CHECK_ERROR(IsRpcResponse(rpcMethod), L"ChatBotJsonDispatcherBase expects an RPC request or response.");
			if (!HandleUnmatchedResponse(senderClientId, package))
			{
				CHECK_FAIL(L"ChatBotJsonDispatcherBase received an RPC response while no request is waiting.");
			}
		}
	}

	void ChatBotJsonDispatcherBase::FinalizeRpc()
	{
		if (lifecycle)
		{
			lifecycle->Finalize();
			lifecycle = nullptr;
			rpcDispatcher = nullptr;
			ops = nullptr;
		}
	}

/***********************************************************************
ChatBotJsonDispatcherClient
***********************************************************************/

	ChatBotJsonDispatcherClient::ChatBotJsonDispatcherClient(JsonChannel* channel)
		: ChatBotJsonDispatcherBase(channel)
	{
	}

	void ChatBotJsonDispatcherClient::SendJsonRequest(JsonPackage message, bool broadcast)
	{
		if (broadcast)
		{
			CHECK_ERROR(serverLocalClientId != -1, L"ChatBotJsonDispatcherClient has not logged in.");
			GetRpcChannel()->SendToClient(serverLocalClientId, message);
		}
		else
		{
			GetRpcChannel()->SendToClient(ReadTargetClientId(message), message);
		}
		FlushChannel();
	}

	void ChatBotJsonDispatcherClient::LoginClient(vint clientId)
	{
		auto loginMessage = PopReceivedMessage();
		CHECK_ERROR(TryReadLoginMessage(loginMessage.message, serverLocalClientId), L"ChatBotJsonDispatcherClient expects a login message.");
		CHECK_ERROR(loginMessage.senderClientId == serverLocalClientId, L"ChatBotJsonDispatcherClient login message should be sent by the server local client.");

		InitializeRpc(clientId);
		lifecycle->DeclareRemoteService(GetChatServerTypeId(), serverLocalClientId);
		lifecycle->Initialize();

		chatServer = lifecycle->RequestService(WString::Unmanaged(L"chatapi::IChatServer")).Cast<chatapi::IChatServer>();
		CHECK_ERROR(chatServer, L"ChatBotJsonDispatcherClient failed to request IChatServer.");
	}

	Ptr<chatapi::IChatServer> ChatBotJsonDispatcherClient::GetChatServer()
	{
		return chatServer;
	}

/***********************************************************************
ChatBotJsonDispatcherServer
***********************************************************************/

	ChatBotJsonDispatcherServer::ChatBotJsonDispatcherServer(JsonChannel* channel, Ptr<chatapi::IChatServer> chatServerService)
		: ChatBotJsonDispatcherBase(channel)
		, service(chatServerService)
	{
		CHECK_ERROR(service, L"ChatBotJsonDispatcherServer needs an IChatServer service.");
	}

	WString ChatBotJsonDispatcherServer::MakeBroadcastKey(vint clientId, vint requestId)
	{
		return itow(clientId) + WString::Unmanaged(L":") + itow(requestId);
	}

	JsonPackage ChatBotJsonDispatcherServer::CreateBroadcastResponse(vint sourceClientId, vint targetClientId, vint requestId, Ptr<PendingBroadcast> pending)
	{
		auto response = CreateRpcMessage(WString::Unmanaged(L"Response:Broadcast_Response"), requestId, sourceClientId);
		AddJsonObjectField(response, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(targetClientId));

		if (pending && pending->hasNonNullResponse)
		{
			auto consolidated = CreateJsonArray();
			for (auto clientId : pending->expectedClientIds)
			{
				auto index = pending->responses.Keys().IndexOf(clientId);
				if (index != -1)
				{
					auto responseObject = GetJsonObject(pending->responses.Values()[index]);
					auto responseField = FindJsonObjectField(responseObject, WString::Unmanaged(L"response"));
					if (responseField && !IsJsonNull(responseField))
					{
						auto array = GetJsonArray(responseField);
						for (auto item : array->items)
						{
							consolidated->items.Add(item);
						}
					}
				}
			}
			AddJsonObjectField(response, WString::Unmanaged(L"response"), consolidated);
		}
		else
		{
			AddJsonObjectField(response, WString::Unmanaged(L"response"), CreateJsonLiteral(JsonLiteralValue::Null));
		}
		return response;
	}

	ChatBotJsonDispatcherServer::CompletedBroadcast ChatBotJsonDispatcherServer::CompleteBroadcastLocked(const WString& key)
	{
		auto pending = pendingBroadcasts[key];
		auto serverClientId = GetServerClientId();
		CompletedBroadcast completed;
		completed.originalClientId = pending->originalClientId;
		completed.response = CreateBroadcastResponse(serverClientId, pending->originalClientId, pending->originalRequestId, pending);

		redirectedBroadcasts.Remove(pending->redirectedRequestId);
		pendingBroadcasts.Remove(key);
		return completed;
	}

	void ChatBotJsonDispatcherServer::DeliverCompletedBroadcast(const CompletedBroadcast& completed)
	{
		if (!completed.response)
		{
			return;
		}

		auto serverClientId = GetServerClientId();
		if (completed.originalClientId == serverClientId)
		{
			PushReceivedMessage(serverClientId, completed.response);
		}
		else
		{
			SendJsonResponse(completed.originalClientId, completed.response);
		}
	}

	JsonPackage ChatBotJsonDispatcherServer::StartBroadcast(vint originalClientId, vint originalRequestId, JsonPackage message)
	{
		auto serverClientId = GetServerClientId();
		auto redirectedRequestId = AllocateRequestId();
		copy_visitor::AstVisitor copier;
		auto redirectedMessage = copier.CopyNode(message.Obj());
		WriteRequestId(redirectedMessage, redirectedRequestId);
		WriteSourceClientId(redirectedMessage, serverClientId);

		JsonPackage immediateResponse;
		bool shouldFlush = false;
		List<vint> blockedReceivers;
		auto pending = Ptr(new PendingBroadcast);
		pending->originalClientId = originalClientId;
		pending->originalRequestId = originalRequestId;
		pending->redirectedRequestId = redirectedRequestId;

		SPIN_LOCK(lockBroadcasts)
		{
			for (auto clientId : connectedClientIds)
			{
				if (clientId != originalClientId)
				{
					pending->expectedClientIds.Add(clientId);
				}
			}

			if (pending->expectedClientIds.Count() == 0)
			{
				immediateResponse = CreateBroadcastResponse(serverClientId, originalClientId, originalRequestId, nullptr);
			}
			else
			{
				auto key = MakeBroadcastKey(originalClientId, originalRequestId);
				pendingBroadcasts.Add(key, pending);
				redirectedBroadcasts.Add(redirectedRequestId, key);

				if (connectedClientIds.Contains(originalClientId))
				{
					blockedReceivers.Add(originalClientId);
				}
				GetRpcChannel()->BroadcastFromClient(redirectedMessage, blockedReceivers);
				shouldFlush = true;
			}
		}

		if (shouldFlush)
		{
			FlushChannel();
		}
		return immediateResponse;
	}

	bool ChatBotJsonDispatcherServer::TryHandleBroadcastResponse(vint senderClientId, JsonPackage response)
	{
		if (TryReadRpcMethod(response) != WString::Unmanaged(L"Response:Broadcast_Response"))
		{
			return false;
		}

		CompletedBroadcast completed;
		auto redirectedRequestId = ReadRequestId(response);
		bool handled = false;

		SPIN_LOCK(lockBroadcasts)
		{
			auto index = redirectedBroadcasts.Keys().IndexOf(redirectedRequestId);
			if (index != -1)
			{
				handled = true;
				auto key = redirectedBroadcasts.Values()[index];
				auto pending = pendingBroadcasts[key];

				if (pending->expectedClientIds.Contains(senderClientId))
				{
					auto responseObject = GetJsonObject(response);
					auto responseField = FindJsonObjectField(responseObject, WString::Unmanaged(L"response"));
					if (responseField && !IsJsonNull(responseField))
					{
						pending->hasNonNullResponse = true;
					}
					pending->responses.Set(senderClientId, response);

					if (IsBroadcastReady(pending))
					{
						completed = CompleteBroadcastLocked(key);
					}
				}
			}
		}

		DeliverCompletedBroadcast(completed);
		return handled;
	}

	void ChatBotJsonDispatcherServer::SendLoginMessage(vint clientId)
	{
		GetRpcChannel()->SendToClient(clientId, CreateLoginMessage(GetServerClientId()));
		FlushChannel();
	}

	void ChatBotJsonDispatcherServer::SendJsonRequest(JsonPackage message, bool broadcast)
	{
		if (broadcast)
		{
			auto immediateResponse = StartBroadcast(ReadSourceClientId(message), ReadRequestId(message), message);
			if (immediateResponse)
			{
				PushReceivedMessage(GetServerClientId(), immediateResponse);
			}
		}
		else
		{
			GetRpcChannel()->SendToClient(ReadTargetClientId(message), message);
			FlushChannel();
		}
	}

	bool ChatBotJsonDispatcherServer::HandleIncomingRequest(vint senderClientId, JsonPackage request)
	{
		auto rpcMethod = TryReadRpcMethod(request);
		if (!IsBroadcastRequest(rpcMethod) || senderClientId == GetServerClientId())
		{
			return false;
		}

		auto originalClientId = ReadSourceClientId(request);
		CHECK_ERROR(originalClientId == senderClientId, L"ChatBotJsonDispatcherServer received a broadcast request with a mismatched source client id.");
		auto immediateResponse = StartBroadcast(originalClientId, ReadRequestId(request), request);
		if (immediateResponse)
		{
			SendJsonResponse(originalClientId, immediateResponse);
		}
		return true;
	}

	bool ChatBotJsonDispatcherServer::HandleUnmatchedResponse(vint senderClientId, JsonPackage response)
	{
		return TryHandleBroadcastResponse(senderClientId, response);
	}

	void ChatBotJsonDispatcherServer::AfterRequestResponse(vint senderClientId, JsonPackage request)
	{
		if (senderClientId != GetServerClientId() && IsRemoveUserRequest(request))
		{
			DisconnectClient(senderClientId);
		}
	}

	void ChatBotJsonDispatcherServer::RegisterLocalClient(vint clientId)
	{
		CHECK_ERROR(clientId != -1, L"ChatBotJsonDispatcherServer needs a valid local client id.");

		List<vint> pendingClientIds;
		SPIN_LOCK(lockClients)
		{
			CHECK_ERROR(localClientId == -1, L"ChatBotJsonDispatcherServer local client has already been registered.");
			localClientId = clientId;
			for (auto pendingClientId : pendingLoginClientIds)
			{
				pendingClientIds.Add(pendingClientId);
			}
			pendingLoginClientIds.Clear();
		}

		InitializeRpc(clientId);
		lifecycle->RegisterLocalService(GetChatServerTypeId(), service);
		lifecycle->Initialize();

		SPIN_LOCK(lockBroadcasts)
		{
			for (auto pendingClientId : pendingClientIds)
			{
				if (!connectedClientIds.Contains(pendingClientId))
				{
					connectedClientIds.Add(pendingClientId);
				}
			}
		}

		for (auto pendingClientId : pendingClientIds)
		{
			SchedulaTask(Func<void()>([this, pendingClientId]()
			{
				SendLoginMessage(pendingClientId);
			}));
		}
	}

	void ChatBotJsonDispatcherServer::RegisterClient(vint clientId)
	{
		CHECK_ERROR(clientId != -1, L"ChatBotJsonDispatcherServer needs a valid client id.");

		bool localClientReady = false;
		SPIN_LOCK(lockClients)
		{
			localClientReady = localClientId != -1;
			if (!localClientReady && !pendingLoginClientIds.Contains(clientId))
			{
				pendingLoginClientIds.Add(clientId);
			}
		}

		if (!localClientReady)
		{
			return;
		}

		SPIN_LOCK(lockBroadcasts)
		{
			if (!connectedClientIds.Contains(clientId))
			{
				connectedClientIds.Add(clientId);
			}
		}

		SchedulaTask(Func<void()>([this, clientId]()
		{
			SendLoginMessage(clientId);
		}));
	}

	void ChatBotJsonDispatcherServer::DisconnectClient(vint clientId)
	{
		bool isLocalClient = false;
		SPIN_LOCK(lockClients)
		{
			isLocalClient = clientId == localClientId;
			auto pendingIndex = pendingLoginClientIds.IndexOf(clientId);
			if (pendingIndex != -1)
			{
				pendingLoginClientIds.RemoveAt(pendingIndex);
			}
		}

		if (isLocalClient)
		{
			return;
		}

		List<CompletedBroadcast> completedBroadcasts;
		SPIN_LOCK(lockBroadcasts)
		{
			if (connectedClientIds.Contains(clientId))
			{
				connectedClientIds.Remove(clientId);
			}

			for (vint i = pendingBroadcasts.Count(); i > 0; i--)
			{
				auto index = i - 1;
				auto key = pendingBroadcasts.Keys().Get(index);
				auto pending = pendingBroadcasts.Values().Get(index);
				auto expectedIndex = pending->expectedClientIds.IndexOf(clientId);
				if (expectedIndex != -1)
				{
					pending->expectedClientIds.RemoveAt(expectedIndex);
					if (IsBroadcastReady(pending))
					{
						completedBroadcasts.Add(CompleteBroadcastLocked(key));
					}
				}
			}
		}

		for (auto&& completed : completedBroadcasts)
		{
			DeliverCompletedBroadcast(completed);
		}
	}

	vint ChatBotJsonDispatcherServer::GetServerClientId()
	{
		vint result = -1;
		SPIN_LOCK(lockClients)
		{
			result = localClientId;
		}
		CHECK_ERROR(result != -1, L"ChatBotJsonDispatcherServer local client has not been registered.");
		return result;
	}
}
