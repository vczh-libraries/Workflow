#include "ChatBotJsonDispatcherServer.h"

#include <cwchar>

namespace chatbot
{
	using namespace vl;
	using namespace vl::collections;
	using namespace vl::glr::json;

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
			CHECK_ERROR(object, L"ChatBotJsonDispatcherServer expects a JSON object.");
			return object;
		}

		Ptr<JsonArray> GetJsonArray(Ptr<JsonNode> node)
		{
			auto array = node.Cast<JsonArray>();
			CHECK_ERROR(array, L"ChatBotJsonDispatcherServer expects a JSON array.");
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
			CHECK_ERROR(field, L"ChatBotJsonDispatcherServer expects a JSON object field.");
			return field;
		}

		WString GetJsonString(Ptr<JsonNode> node)
		{
			auto stringNode = node.Cast<JsonString>();
			CHECK_ERROR(stringNode, L"ChatBotJsonDispatcherServer expects a JSON string.");
			return stringNode->content.value;
		}

		vint GetJsonInt(Ptr<JsonNode> node)
		{
			auto numberNode = node.Cast<JsonNumber>();
			CHECK_ERROR(numberNode, L"ChatBotJsonDispatcherServer expects a JSON number.");
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
			return rpcMethod == WString::Unmanaged(L"Request:IObjectEventOps_InvokeEvent");
		}

		bool IsBroadcastAndDropRequest(const WString& rpcMethod)
		{
			return rpcMethod == WString::Unmanaged(L"Request:IRpcDispatcher_DeclareRemoteService");
		}

		vint ReadRequestId(Ptr<JsonNode> message)
		{
			return GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"rpcRequestId")));
		}

		void WriteRequestId(Ptr<JsonNode> message, vint requestId)
		{
			SetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"rpcRequestId"), CreateJsonNumber(requestId));
		}

		vint ReadSourceClientId(Ptr<JsonNode> message)
		{
			return GetJsonInt(GetJsonObjectField(GetJsonObject(message), WString::Unmanaged(L"sourceClientId")));
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

		bool TryReadLogoutMessage(Ptr<JsonNode> message)
		{
			if (auto object = message.Cast<JsonObject>())
			{
				auto systemField = FindJsonObjectField(object, WString::Unmanaged(L"chatBotSystem"));
				return systemField && GetJsonString(systemField) == WString::Unmanaged(L"Logout");
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
ChatBotJsonDispatcherServer
***********************************************************************/

	ChatBotJsonDispatcherServer::ChatBotJsonDispatcherServer(JsonChannelClient* _serverClient, JsonChannel* channel)
	{
		CHECK_ERROR(_serverClient, L"ChatBotJsonDispatcherServer needs a server channel client.");
		CHECK_ERROR(channel, L"ChatBotJsonDispatcherServer needs an RPC channel.");
		serverClient = _serverClient;
		rpcChannel = channel;
		rpcChannel->Initialize(this);
	}

	vint ChatBotJsonDispatcherServer::AllocateRequestId()
	{
		return ++nextRequestId;
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
		if (completed.response)
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
				CHECK_ERROR(rpcChannel, L"ChatBotJsonDispatcherServer needs an RPC channel.");
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

	void ChatBotJsonDispatcherServer::StartBroadcastAndDrop(vint originalClientId, JsonPackage message)
	{
		List<vint> blockedReceivers;
		SPIN_LOCK(lockBroadcasts)
		{
			if (connectedClientIds.Contains(originalClientId))
			{
				blockedReceivers.Add(originalClientId);
			}
			CHECK_ERROR(rpcChannel, L"ChatBotJsonDispatcherServer needs an RPC channel.");
			rpcChannel->BroadcastFromClient(message, blockedReceivers);
		}
		FlushChannel();
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

	void ChatBotJsonDispatcherServer::HandleServiceDeclaration(vint senderClientId, JsonPackage request)
	{
		SPIN_LOCK(lockBroadcasts)
		{
			cachedServiceDeclarations.Add(request);
		}
		StartBroadcastAndDrop(senderClientId, request);
	}

	void ChatBotJsonDispatcherServer::SendLoginMessages(vint clientId)
	{
		List<JsonPackage> declarations;
		SPIN_LOCK(lockBroadcasts)
		{
			for (auto request : cachedServiceDeclarations)
			{
				declarations.Add(request);
			}
		}

		CHECK_ERROR(rpcChannel, L"ChatBotJsonDispatcherServer needs an RPC channel.");
		rpcChannel->SendToClient(clientId, CreateLoginMessage(GetServerClientId()));
		for (auto request : declarations)
		{
			rpcChannel->SendToClient(clientId, request);
		}
		FlushChannel();
	}

	void ChatBotJsonDispatcherServer::SendJsonResponse(vint receiverClientId, JsonPackage response)
	{
		CHECK_ERROR(rpcChannel, L"ChatBotJsonDispatcherServer needs an RPC channel.");
		rpcChannel->SendToClient(receiverClientId, response);
		FlushChannel();
	}

	void ChatBotJsonDispatcherServer::FlushChannel()
	{
		CHECK_ERROR(rpcChannel, L"ChatBotJsonDispatcherServer needs an RPC channel.");
		bool disconnected = false;
		rpcChannel->BatchWrite(disconnected);
	}

	bool ChatBotJsonDispatcherServer::HasServerClientId()
	{
		return serverClient->GetClientId() != -1;
	}

	void ChatBotJsonDispatcherServer::RegisterClient(vint clientId)
	{
		CHECK_ERROR(clientId != -1, L"ChatBotJsonDispatcherServer needs a valid client id.");
		CHECK_ERROR(HasServerClientId(), L"ChatBotJsonDispatcherServer needs its server local client to connect first.");

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

	void ChatBotJsonDispatcherServer::DisconnectClient(vint clientId)
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
		auto result = serverClient->GetClientId();
		CHECK_ERROR(result != -1, L"ChatBotJsonDispatcherServer server client has not been connected.");
		return result;
	}

	void ChatBotJsonDispatcherServer::OnRead(vint senderClientId, const JsonPackage& package)
	{
		if (TryReadLogoutMessage(package))
		{
			DisconnectClient(senderClientId);
			return;
		}

		auto rpcMethod = TryReadRpcMethod(package);
		CHECK_ERROR(rpcMethod != WString::Empty, L"ChatBotJsonDispatcherServer expects an RPC message.");

		if (IsRpcRequest(rpcMethod))
		{
			auto originalClientId = ReadSourceClientId(package);
			CHECK_ERROR(originalClientId == senderClientId, L"ChatBotJsonDispatcherServer received a request with a mismatched source client id.");

			if (IsBroadcastAndDropRequest(rpcMethod))
			{
				HandleServiceDeclaration(originalClientId, package);
			}
			else if (IsBroadcastRequest(rpcMethod))
			{
				auto immediateResponse = StartBroadcast(originalClientId, ReadRequestId(package), package);
				if (immediateResponse)
				{
					SendJsonResponse(originalClientId, immediateResponse);
				}
			}
			else
			{
				CHECK_FAIL(L"ChatBotJsonDispatcherServer only accepts broadcast requests.");
			}
			return;
		}

		CHECK_ERROR(IsRpcResponse(rpcMethod), L"ChatBotJsonDispatcherServer expects an RPC request or response.");
		if (!TryHandleBroadcastResponse(senderClientId, package))
		{
			CHECK_FAIL(L"ChatBotJsonDispatcherServer received an unmatched RPC response.");
		}
	}
}
