#include "RpcJsonDispatcherClient.h"

#include <cwchar>

namespace vl::rpc_controller::channeling
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
			CHECK_ERROR(object, L"RpcJsonDispatcherClient expects a JSON object.");
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
			CHECK_ERROR(field, L"RpcJsonDispatcherClient expects a JSON object field.");
			return field;
		}

		WString GetJsonString(Ptr<JsonNode> node)
		{
			auto stringNode = node.Cast<JsonString>();
			CHECK_ERROR(stringNode, L"RpcJsonDispatcherClient expects a JSON string.");
			return stringNode->content.value;
		}

		vint GetJsonInt(Ptr<JsonNode> node)
		{
			auto numberNode = node.Cast<JsonNumber>();
			CHECK_ERROR(numberNode, L"RpcJsonDispatcherClient expects a JSON number.");
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
			CHECK_FAIL(L"RpcJsonDispatcherClient received an unknown request method.");
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
			AddJsonObjectField(message, WString::Unmanaged(L"rpcChannelingSystem"), CreateJsonString(WString::Unmanaged(L"Login")));
			AddJsonObjectField(message, WString::Unmanaged(L"serverClientId"), CreateJsonNumber(serverClientId));
			return message;
		}

		Ptr<JsonObject> CreateLogoutMessage()
		{
			auto message = CreateJsonObject();
			AddJsonObjectField(message, WString::Unmanaged(L"rpcChannelingSystem"), CreateJsonString(WString::Unmanaged(L"Logout")));
			return message;
		}

		bool TryReadLoginMessage(Ptr<JsonNode> message, vint& serverClientId)
		{
			if (auto object = message.Cast<JsonObject>())
			{
				auto systemField = FindJsonObjectField(object, WString::Unmanaged(L"rpcChannelingSystem"));
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
RpcJsonDispatcherClient
***********************************************************************/

	RpcJsonDispatcherClient::RpcJsonDispatcherClient()
	{
		CHECK_ERROR(semaphoreMessages.Create(0, 65536), L"RpcJsonDispatcherClient failed to create the message semaphore.");
		CHECK_ERROR(eventServerLocalClientId.CreateManualUnsignal(false), L"RpcJsonDispatcherClient failed to create the server-client-id event.");
	}

	void RpcJsonDispatcherClient::PrepareConnection(JsonChannel* channel, const List<WString>& _waitingForServices)
	{
		CHECK_ERROR(channel, L"RpcJsonDispatcherClient needs an RPC channel.");
		CHECK_ERROR(!rpcChannel, L"RpcJsonDispatcherClient connection can only be initialized once.");
		rpcChannel = channel;
		CopyFrom(waitingForServices, _waitingForServices);
		if (waitingForServices.Count() > 0)
		{
			eventWaitingForServicesCreated = true;
			CHECK_ERROR(eventWaitingForServices.CreateManualUnsignal(false), L"RpcJsonDispatcherClient failed to create the service-waiting event.");
		}
		rpcChannel->Initialize(this);
	}

	void RpcJsonDispatcherClient::SetRpcObjects(Ptr<RpcJsonDispatcher> _rpcDispatcher, Ptr<RpcJsonLifecycle> _lifecycle)
	{
		CHECK_ERROR(_rpcDispatcher, L"RpcJsonDispatcherClient needs a dispatcher.");
		CHECK_ERROR(_lifecycle, L"RpcJsonDispatcherClient needs a lifecycle.");
		CHECK_ERROR(!rpcDispatcher, L"RpcJsonDispatcherClient RPC objects can only be initialized once.");
		rpcDispatcher = _rpcDispatcher;
		lifecycle = _lifecycle;
	}

	RpcJsonLifecycle* RpcJsonDispatcherClient::GetRpcJsonLifecycle()
	{
		CHECK_ERROR(lifecycle, L"RpcJsonDispatcherClient needs a lifecycle.");
		return lifecycle.Obj();
	}

	void RpcJsonDispatcherClient::ProcessCachedIncomingServiceDeclarations()
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

	void RpcJsonDispatcherClient::SendCachedOutgoingServiceDeclarations()
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

	void RpcJsonDispatcherClient::ProcessIncomingServiceDeclaration(JsonPackage request)
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

	void RpcJsonDispatcherClient::UpdateWaitingForServices(JsonPackage request)
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

	void RpcJsonDispatcherClient::WaitForServerClientId()
	{
		if (serverLocalClientId.load() == -1)
		{
			eventServerLocalClientId.Wait();
		}
	}

	void RpcJsonDispatcherClient::WaitForExpectedServices()
	{
		if (eventWaitingForServicesCreated)
		{
			eventWaitingForServices.Wait();
		}
	}

	void RpcJsonDispatcherClient::SendJsonRequest(JsonPackage message, RequestType requestType)
	{
		CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherClient needs an RPC channel.");
		switch (requestType)
		{
		case RequestType::Direct:
			rpcChannel->SendToClient(ReadTargetClientId(message), message);
			break;
		case RequestType::Broadcast:
		case RequestType::BroadcastAndDrop:
			WaitForServerClientId();
			rpcChannel->SendToClient(serverLocalClientId.load(), message);
			break;
		default:
			CHECK_FAIL(L"RpcJsonDispatcherClient received an unknown request type.");
		}
		FlushChannel();
	}

	void RpcJsonDispatcherClient::PushReceivedMessage(vint senderClientId, JsonPackage message)
	{
		SPIN_LOCK(lockMessages)
		{
			messages.Add({ senderClientId, message });
		}
		semaphoreMessages.Release();
	}

	RpcJsonDispatcherClient::ReceivedJsonMessage RpcJsonDispatcherClient::PopReceivedMessage()
	{
		semaphoreMessages.Wait();
		ReceivedJsonMessage result;
		SPIN_LOCK(lockMessages)
		{
			CHECK_ERROR(messages.Count() > 0, L"RpcJsonDispatcherClient message queue is unexpectedly empty.");
			result = messages[0];
			messages.RemoveAt(0);
		}
		return result;
	}

	bool RpcJsonDispatcherClient::TryPopBufferedResponse(vint requestId, ReceivedJsonMessage& message)
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

	void RpcJsonDispatcherClient::PushBufferedResponse(ReceivedJsonMessage message)
	{
		SPIN_LOCK(lockMessages)
		{
			bufferedResponses.Add(message);
		}
	}

	void RpcJsonDispatcherClient::SendJsonResponse(vint receiverClientId, JsonPackage response)
	{
		if (response)
		{
			CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherClient needs an RPC channel.");
			rpcChannel->SendToClient(receiverClientId, response);
			FlushChannel();
		}
	}

	void RpcJsonDispatcherClient::FlushChannel()
	{
		CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherClient needs an RPC channel.");
		bool disconnected = false;
		rpcChannel->BatchWrite(disconnected);
	}

	void RpcJsonDispatcherClient::ProcessRequestAndSendResponse(vint senderClientId, JsonPackage request)
	{
		auto response = TranslateRequest(request);
		SendJsonResponse(senderClientId, response);
	}

	JsonPackage RpcJsonDispatcherClient::TranslateRequest(JsonPackage request)
	{
		auto rpcMethod = TryReadRpcMethod(request);
		CHECK_ERROR(IsRpcRequest(rpcMethod), L"RpcJsonDispatcherClient expects an RPC request.");

		return IRpcJsonMessageDispatcher::DefaultTranslate(
			request,
			GetRequestType(rpcMethod),
			lifecycle->GetController()->GetObjectOps(),
			lifecycle->GetController()->GetObjectEventOps(),
			lifecycle->GetDispatcher(),
			lifecycle.Obj()
			);
	}

	void RpcJsonDispatcherClient::WaitForServer(JsonChannelClient* channelClient, JsonChannel* channel, const List<WString>& _waitingForServices)
	{
		CHECK_ERROR(channelClient, L"RpcJsonDispatcherClient needs a channel client.");
		PrepareConnection(channel, _waitingForServices);
		channelClient->WaitForServer();
		CHECK_ERROR(channelClient->GetStatus() == ClientStatus::Connected, L"RpcJsonDispatcherClient failed to connect to the server.");
	}

	vint RpcJsonDispatcherClient::ConnectLocalServer(JsonChannelServer* channelServer, Ptr<JsonChannelClient> localClient, JsonChannel* channel, const List<WString>& _waitingForServices)
	{
		CHECK_ERROR(channelServer, L"RpcJsonDispatcherClient needs a channel server.");
		CHECK_ERROR(localClient, L"RpcJsonDispatcherClient needs a local client.");
		PrepareConnection(channel, _waitingForServices);
		auto clientId = channelServer->ConnectLocalClient(localClient);
		CHECK_ERROR(clientId != -1, L"RpcJsonDispatcherClient failed to connect the local client.");
		return clientId;
	}

	void RpcJsonDispatcherClient::Initialize()
	{
		CHECK_ERROR(lifecycle, L"RpcJsonDispatcherClient needs to connect before Initialize.");
		CHECK_ERROR(initialized.load() == 0, L"RpcJsonDispatcherClient can only be initialized once.");
		ProcessCachedIncomingServiceDeclarations();
		lifecycle->Initialize();
		initialized.store(1);
		SendCachedOutgoingServiceDeclarations();
		WaitForExpectedServices();
	}

	IRpcLifecycle* RpcJsonDispatcherClient::GetRpcLifecycle()
	{
		CHECK_ERROR(lifecycle, L"RpcJsonDispatcherClient needs a lifecycle.");
		return lifecycle.Obj();
	}

	IRpcDispatcher* RpcJsonDispatcherClient::GetRpcDispatcher()
	{
		CHECK_ERROR(rpcDispatcher, L"RpcJsonDispatcherClient needs a dispatcher.");
		return rpcDispatcher.Obj();
	}

	vint RpcJsonDispatcherClient::AllocateRequestId()
	{
		return ++nextRequestId;
	}

	JsonPackage RpcJsonDispatcherClient::OnJsonRequest(JsonPackage message, RequestType requestType)
	{
		auto rpcMethod = TryReadRpcMethod(message);
		CHECK_ERROR(rpcMethod != WString::Empty, L"RpcJsonDispatcherClient expects an RPC message.");

		if (requestType == RequestType::BroadcastAndDrop)
		{
			CHECK_ERROR(IsDeclareRemoteServiceRequest(rpcMethod), L"RpcJsonDispatcherClient expects a service declaration.");
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

		CHECK_ERROR(initialized.load() != 0, L"RpcJsonDispatcherClient only accepts service declarations before Initialize.");

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
			CHECK_ERROR(rpcMethod != WString::Empty, L"RpcJsonDispatcherClient expects an RPC message.");

			if (IsRpcRequest(rpcMethod))
			{
				ProcessRequestAndSendResponse(item.senderClientId, item.message);
				continue;
			}

			CHECK_ERROR(IsRpcResponse(rpcMethod), L"RpcJsonDispatcherClient expects an RPC request or response.");
			if (ReadRequestId(item.message) == requestId)
			{
				return item.message;
			}

			PushBufferedResponse(item);
		}
	}

	void RpcJsonDispatcherClient::OnRead(vint senderClientId, const JsonPackage& package)
	{
		vint newServerLocalClientId = -1;
		if (TryReadLoginMessage(package, newServerLocalClientId))
		{
			serverLocalClientId.store(newServerLocalClientId);
			eventServerLocalClientId.Signal();
			return;
		}

		auto rpcMethod = TryReadRpcMethod(package);
		CHECK_ERROR(rpcMethod != WString::Empty, L"RpcJsonDispatcherClient expects an RPC message.");

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

		CHECK_ERROR(initialized.load() != 0, L"RpcJsonDispatcherClient only accepts service declarations before Initialize.");

		if (activeJsonRequests.load() > 0)
		{
			PushReceivedMessage(senderClientId, package);
			return;
		}

		if (IsRpcRequest(rpcMethod))
		{
			ScheduleTask(Func<void()>([this, senderClientId, package]()
			{
				ProcessRequestAndSendResponse(senderClientId, package);
			}));
		}
		else
		{
			CHECK_ERROR(IsRpcResponse(rpcMethod), L"RpcJsonDispatcherClient expects an RPC request or response.");
			CHECK_FAIL(L"RpcJsonDispatcherClient received an RPC response while no request is waiting.");
		}
	}

	void RpcJsonDispatcherClient::FinalizeRpc()
	{
		if (lifecycle)
		{
			lifecycle->Finalize();
			lifecycle = nullptr;
			rpcDispatcher = nullptr;
		}
	}

	void RpcJsonDispatcherClient::SetServerLocalClientId(vint clientId)
	{
		CHECK_ERROR(clientId != -1, L"RpcJsonDispatcherClient needs a valid server client id.");
		serverLocalClientId.store(clientId);
		eventServerLocalClientId.Signal();
	}

	void RpcJsonDispatcherClient::NotifyServerClientDisconnected()
	{
		CHECK_ERROR(rpcChannel, L"RpcJsonDispatcherClient needs an RPC channel.");
		WaitForServerClientId();
		rpcChannel->SendToClient(serverLocalClientId.load(), CreateLogoutMessage());
		FlushChannel();
	}

/***********************************************************************
RpcJsonDispatcherClientForTaskQueue
***********************************************************************/

	void RpcJsonDispatcherClientForTaskQueue::ScheduleTask(Func<void()> task)
	{
		taskQueue->QueueTask(task);
	}

	RpcJsonDispatcherClientForTaskQueue::RpcJsonDispatcherClientForTaskQueue(Ptr<TaskQueue> _taskQueue)
		: taskQueue(_taskQueue)
	{
		CHECK_ERROR(taskQueue, L"RpcJsonDispatcherClientForTaskQueue needs a task queue.");
	}
}
