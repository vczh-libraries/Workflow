#include "WfLibraryRpcJsonDispatcherServer.h"

#include <cwchar>

namespace vl::rpc_controller::channeling
{
	using namespace vl;
	using namespace vl::collections;
	using namespace vl::glr::json;

	namespace
	{
		Ptr<JsonLiteral> ServerCreateJsonLiteral(JsonLiteralValue value)
		{
			auto node = Ptr(new JsonLiteral);
			node->value = value;
			return node;
		}

		Ptr<JsonString> ServerCreateJsonString(const WString& value)
		{
			auto node = Ptr(new JsonString);
			node->content.value = value;
			return node;
		}

		Ptr<JsonNumber> ServerCreateJsonNumber(vint value)
		{
			auto node = Ptr(new JsonNumber);
			node->content.value = itow(value);
			return node;
		}

		Ptr<JsonArray> ServerCreateJsonArray()
		{
			return Ptr(new JsonArray);
		}

		Ptr<JsonObject> ServerCreateJsonObject()
		{
			return Ptr(new JsonObject);
		}

		Ptr<JsonObject> ServerGetJsonObject(Ptr<JsonNode> node)
		{
			auto object = node.Cast<JsonObject>();
			CHECK_ERROR(object, L"RpcJsonDispatcherServer expects a JSON object.");
			return object;
		}

		Ptr<JsonArray> ServerGetJsonArray(Ptr<JsonNode> node)
		{
			auto array = node.Cast<JsonArray>();
			CHECK_ERROR(array, L"RpcJsonDispatcherServer expects a JSON array.");
			return array;
		}

		void ServerAddJsonObjectField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
		{
			auto field = Ptr(new JsonObjectField);
			field->name.value = name;
			field->value = value;
			object->fields.Add(field);
		}

		void ServerSetJsonObjectField(Ptr<JsonObject> object, const WString& name, Ptr<JsonNode> value)
		{
			for (auto field : object->fields)
			{
				if (field->name.value == name)
				{
					field->value = value;
					return;
				}
			}
			ServerAddJsonObjectField(object, name, value);
		}

		Ptr<JsonNode> ServerFindJsonObjectField(Ptr<JsonObject> object, const WString& name)
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

		Ptr<JsonNode> ServerGetJsonObjectField(Ptr<JsonObject> object, const WString& name)
		{
			auto field = ServerFindJsonObjectField(object, name);
			CHECK_ERROR(field, L"RpcJsonDispatcherServer expects a JSON object field.");
			return field;
		}

		WString ServerGetJsonString(Ptr<JsonNode> node)
		{
			auto stringNode = node.Cast<JsonString>();
			CHECK_ERROR(stringNode, L"RpcJsonDispatcherServer expects a JSON string.");
			return stringNode->content.value;
		}

		vint ServerGetJsonInt(Ptr<JsonNode> node)
		{
			auto numberNode = node.Cast<JsonNumber>();
			CHECK_ERROR(numberNode, L"RpcJsonDispatcherServer expects a JSON number.");
			return wtoi(numberNode->content.value);
		}

		bool ServerIsJsonNull(Ptr<JsonNode> node)
		{
			auto literal = node.Cast<JsonLiteral>();
			return literal && literal->value == JsonLiteralValue::Null;
		}

		bool ServerStartsWith(const WString& value, const wchar_t* prefix)
		{
			auto prefixLength = (vint)wcslen(prefix);
			return value.Length() >= prefixLength && wcsncmp(value.Buffer(), prefix, prefixLength) == 0;
		}

		WString ServerTryReadRpcMethod(Ptr<JsonNode> message)
		{
			if (auto object = message.Cast<JsonObject>())
			{
				if (auto field = ServerFindJsonObjectField(object, WString::Unmanaged(L"rpcMethod")))
				{
					if (auto stringNode = field.Cast<JsonString>())
					{
						return stringNode->content.value;
					}
				}
			}
			return WString::Empty;
		}

		bool ServerIsRpcRequest(const WString& rpcMethod)
		{
			return ServerStartsWith(rpcMethod, L"Request:");
		}

		bool ServerIsRpcResponse(const WString& rpcMethod)
		{
			return ServerStartsWith(rpcMethod, L"Response:");
		}

		bool ServerIsBroadcastRequest(const WString& rpcMethod)
		{
			return rpcMethod == WString::Unmanaged(L"Request:IObjectEventOps_InvokeEvent");
		}

		bool ServerIsBroadcastAndDropRequest(const WString& rpcMethod)
		{
			return rpcMethod == WString::Unmanaged(L"Request:IRpcDispatcher_DeclareRemoteService");
		}

		vint ServerReadRequestId(Ptr<JsonNode> message)
		{
			return ServerGetJsonInt(ServerGetJsonObjectField(ServerGetJsonObject(message), WString::Unmanaged(L"rpcRequestId")));
		}

		void ServerWriteRequestId(Ptr<JsonNode> message, vint requestId)
		{
			ServerSetJsonObjectField(ServerGetJsonObject(message), WString::Unmanaged(L"rpcRequestId"), ServerCreateJsonNumber(requestId));
		}

		vint ServerReadSourceClientId(Ptr<JsonNode> message)
		{
			return ServerGetJsonInt(ServerGetJsonObjectField(ServerGetJsonObject(message), WString::Unmanaged(L"sourceClientId")));
		}

		void ServerWriteSourceClientId(Ptr<JsonNode> message, vint clientId)
		{
			ServerSetJsonObjectField(ServerGetJsonObject(message), WString::Unmanaged(L"sourceClientId"), ServerCreateJsonNumber(clientId));
		}

		Ptr<JsonObject> ServerCreateRpcMessage(const WString& rpcMethod, vint requestId, vint sourceClientId)
		{
			auto message = ServerCreateJsonObject();
			ServerAddJsonObjectField(message, WString::Unmanaged(L"rpcMethod"), ServerCreateJsonString(rpcMethod));
			ServerAddJsonObjectField(message, WString::Unmanaged(L"rpcRequestId"), ServerCreateJsonNumber(requestId));
			ServerAddJsonObjectField(message, WString::Unmanaged(L"sourceClientId"), ServerCreateJsonNumber(sourceClientId));
			return message;
		}

		Ptr<JsonObject> ServerCreateLoginMessage(vint serverClientId)
		{
			auto message = ServerCreateJsonObject();
			ServerAddJsonObjectField(message, WString::Unmanaged(L"rpcChannelingSystem"), ServerCreateJsonString(WString::Unmanaged(L"Login")));
			ServerAddJsonObjectField(message, WString::Unmanaged(L"serverClientId"), ServerCreateJsonNumber(serverClientId));
			return message;
		}

		bool ServerTryReadLogoutMessage(Ptr<JsonNode> message)
		{
			if (auto object = message.Cast<JsonObject>())
			{
				auto systemField = ServerFindJsonObjectField(object, WString::Unmanaged(L"rpcChannelingSystem"));
				return systemField && ServerGetJsonString(systemField) == WString::Unmanaged(L"Logout");
			}
			return false;
		}

		bool ServerIsBroadcastReady(Ptr<RpcJsonDispatcherServer::PendingBroadcast> pending)
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
RpcJsonDispatcherServer
***********************************************************************/

	RpcJsonDispatcherServer::RpcJsonDispatcherServer(JsonChannelClient* _serverClient, JsonChannel* channel)
	{
		CHECK_ERROR(_serverClient, L"RpcJsonDispatcherServer needs a server channel client.");
		CHECK_ERROR(channel, L"RpcJsonDispatcherServer needs an RPC channel.");
		serverClient = _serverClient;
		rpcChannel = channel;
		rpcChannel->Initialize(this);
	}

	vint RpcJsonDispatcherServer::AllocateRequestId()
	{
		return ++nextRequestId;
	}

	WString RpcJsonDispatcherServer::MakeBroadcastKey(vint clientId, vint requestId)
	{
		return itow(clientId) + WString::Unmanaged(L":") + itow(requestId);
	}

	JsonPackage RpcJsonDispatcherServer::CreateBroadcastResponse(vint sourceClientId, vint targetClientId, vint requestId, Ptr<PendingBroadcast> pending)
	{
		auto response = ServerCreateRpcMessage(WString::Unmanaged(L"Response:Broadcast_Response"), requestId, sourceClientId);
		ServerAddJsonObjectField(response, WString::Unmanaged(L"targetClientId"), ServerCreateJsonNumber(targetClientId));

		if (pending && pending->hasNonNullResponse)
		{
			auto consolidated = ServerCreateJsonArray();
			for (auto clientId : pending->expectedClientIds)
			{
				auto index = pending->responses.Keys().IndexOf(clientId);
				if (index != -1)
				{
					auto responseObject = ServerGetJsonObject(pending->responses.Values()[index]);
					auto responseField = ServerFindJsonObjectField(responseObject, WString::Unmanaged(L"response"));
					if (responseField && !ServerIsJsonNull(responseField))
					{
						auto array = ServerGetJsonArray(responseField);
						for (auto item : array->items)
						{
							consolidated->items.Add(item);
						}
					}
				}
			}
			ServerAddJsonObjectField(response, WString::Unmanaged(L"response"), consolidated);
		}
		else
		{
			ServerAddJsonObjectField(response, WString::Unmanaged(L"response"), ServerCreateJsonLiteral(JsonLiteralValue::Null));
		}
		return response;
	}

	RpcJsonDispatcherServer::CompletedBroadcast RpcJsonDispatcherServer::CompleteBroadcastLocked(const WString& key)
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

	void RpcJsonDispatcherServer::DeliverCompletedBroadcast(const CompletedBroadcast& completed)
	{
		if (completed.response)
		{
			SendJsonResponse(completed.originalClientId, completed.response);
		}
	}

	JsonPackage RpcJsonDispatcherServer::StartBroadcast(vint originalClientId, vint originalRequestId, JsonPackage message)
	{
		auto serverClientId = GetServerClientId();
		auto redirectedRequestId = AllocateRequestId();

		copy_visitor::AstVisitor copier;
		auto redirectedMessage = copier.CopyNode(message.Obj());
		ServerWriteRequestId(redirectedMessage, redirectedRequestId);
		ServerWriteSourceClientId(redirectedMessage, serverClientId);

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
				CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherServer needs an RPC channel.");
				rpcChannel->BroadcastFromClient(redirectedMessage, blockedReceivers);
				shouldFlush = true;
			}
		}

		if (shouldFlush)
		{
			FlushChannel();
		}
		return immediateResponse;
	}

	void RpcJsonDispatcherServer::StartBroadcastAndDrop(vint originalClientId, JsonPackage message)
	{
		List<vint> blockedReceivers;
		SPIN_LOCK(lockBroadcasts)
		{
			if (connectedClientIds.Contains(originalClientId))
			{
				blockedReceivers.Add(originalClientId);
			}
			CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherServer needs an RPC channel.");
			rpcChannel->BroadcastFromClient(message, blockedReceivers);
		}
		FlushChannel();
	}

	bool RpcJsonDispatcherServer::TryHandleBroadcastResponse(vint senderClientId, JsonPackage response)
	{
		if (ServerTryReadRpcMethod(response) != WString::Unmanaged(L"Response:Broadcast_Response"))
		{
			return false;
		}

		CompletedBroadcast completed;
		auto redirectedRequestId = ServerReadRequestId(response);
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
					auto responseObject = ServerGetJsonObject(response);
					auto responseField = ServerFindJsonObjectField(responseObject, WString::Unmanaged(L"response"));
					if (responseField && !ServerIsJsonNull(responseField))
					{
						pending->hasNonNullResponse = true;
					}
					pending->responses.Set(senderClientId, response);

					if (ServerIsBroadcastReady(pending))
					{
						completed = CompleteBroadcastLocked(key);
					}
				}
			}
		}

		DeliverCompletedBroadcast(completed);
		return handled;
	}

	void RpcJsonDispatcherServer::HandleServiceDeclaration(vint senderClientId, JsonPackage request)
	{
		SPIN_LOCK(lockBroadcasts)
		{
			cachedServiceDeclarations.Add(request);
		}
		StartBroadcastAndDrop(senderClientId, request);
	}

	void RpcJsonDispatcherServer::SendLoginMessages(vint clientId)
	{
		List<JsonPackage> declarations;
		SPIN_LOCK(lockBroadcasts)
		{
			for (auto request : cachedServiceDeclarations)
			{
				declarations.Add(request);
			}
		}

		CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherServer needs an RPC channel.");
		rpcChannel->SendToClient(clientId, ServerCreateLoginMessage(GetServerClientId()));
		for (auto request : declarations)
		{
			rpcChannel->SendToClient(clientId, request);
		}
		FlushChannel();
	}

	void RpcJsonDispatcherServer::SendJsonResponse(vint receiverClientId, JsonPackage response)
	{
		CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherServer needs an RPC channel.");
		rpcChannel->SendToClient(receiverClientId, response);
		FlushChannel();
	}

	void RpcJsonDispatcherServer::FlushChannel()
	{
		CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherServer needs an RPC channel.");
		bool disconnected = false;
		rpcChannel->BatchWrite(disconnected);
	}

	bool RpcJsonDispatcherServer::HasServerClientId()
	{
		return serverClient->GetClientId() != -1;
	}

	void RpcJsonDispatcherServer::RegisterClient(vint clientId)
	{
		CHECK_ERROR(clientId != -1, L"RpcJsonDispatcherServer needs a valid client id.");
		CHECK_ERROR(HasServerClientId(), L"RpcJsonDispatcherServer needs its server local client to connect first.");

		SPIN_LOCK(lockBroadcasts)
		{
			if (!connectedClientIds.Contains(clientId))
			{
				connectedClientIds.Add(clientId);
			}
		}

		ScheduleTask(Func<void()>([this, clientId]()
		{
			SendLoginMessages(clientId);
		}));
	}

	void RpcJsonDispatcherServer::DisconnectClient(vint clientId)
	{
		if (clientId == GetServerClientId())
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
					if (ServerIsBroadcastReady(pending))
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

	vint RpcJsonDispatcherServer::GetServerClientId()
	{
		auto result = serverClient->GetClientId();
		CHECK_ERROR(result != -1, L"RpcJsonDispatcherServer server client has not been connected.");
		return result;
	}

	void RpcJsonDispatcherServer::OnRead(vint senderClientId, const JsonPackage& package)
	{
		if (ServerTryReadLogoutMessage(package))
		{
			DisconnectClient(senderClientId);
			return;
		}

		auto rpcMethod = ServerTryReadRpcMethod(package);
		CHECK_ERROR(rpcMethod != WString::Empty, L"RpcJsonDispatcherServer expects an RPC message.");

		if (ServerIsRpcRequest(rpcMethod))
		{
			auto originalClientId = ServerReadSourceClientId(package);
			CHECK_ERROR(originalClientId == senderClientId, L"RpcJsonDispatcherServer received a request with a mismatched source client id.");

			if (ServerIsBroadcastAndDropRequest(rpcMethod))
			{
				HandleServiceDeclaration(originalClientId, package);
			}
			else if (ServerIsBroadcastRequest(rpcMethod))
			{
				auto immediateResponse = StartBroadcast(originalClientId, ServerReadRequestId(package), package);
				if (immediateResponse)
				{
					SendJsonResponse(originalClientId, immediateResponse);
				}
			}
			else
			{
				CHECK_FAIL(L"RpcJsonDispatcherServer only accepts broadcast requests.");
			}
			return;
		}

		CHECK_ERROR(ServerIsRpcResponse(rpcMethod), L"RpcJsonDispatcherServer expects an RPC request or response.");
		if (!TryHandleBroadcastResponse(senderClientId, package))
		{
			CHECK_FAIL(L"RpcJsonDispatcherServer received an unmatched RPC response.");
		}
	}

/***********************************************************************
RpcJsonDispatcherServerForTaskQueue
***********************************************************************/

	void RpcJsonDispatcherServerForTaskQueue::ScheduleTask(Func<void()> task)
	{
		taskQueue->QueueTask(task);
	}

	RpcJsonDispatcherServerForTaskQueue::RpcJsonDispatcherServerForTaskQueue(JsonChannelClient* _serverClient, JsonChannel* channel, Ptr<TaskQueue> _taskQueue)
		: RpcJsonDispatcherServer(_serverClient, channel)
		, taskQueue(_taskQueue)
	{
		CHECK_ERROR(taskQueue, L"RpcJsonDispatcherServerForTaskQueue needs a task queue.");
	}
}
