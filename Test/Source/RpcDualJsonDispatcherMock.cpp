#include "RpcDualJsonDispatcherMock.h"
#include "../../Source/Library/Rpc/WfLibraryRpcWrappers.h"

extern vl::WString GetJsonValueOutputPath();
extern vl::WString GetJsonRequestOutputPath();

namespace vl
{
	namespace rpc_controller_test
	{
		using namespace collections;
		using namespace glr::json;
		using namespace reflection::description;
		using namespace rpc_controller;

		namespace
		{
			Ptr<JsonObject> GetJsonObject(Ptr<JsonNode> node)
			{
				auto object = node.Cast<JsonObject>();
				CHECK_ERROR(object, L"JSON object is expected.");
				return object;
			}

			Ptr<JsonNode> GetJsonObjectField(Ptr<JsonObject> object, const WString& name)
			{
				for (auto field : object->fields)
				{
					if (field->name.value == name)
					{
						return field->value;
					}
				}
				CHECK_FAIL((L"JSON object field not found: " + name).Buffer());
				return nullptr;
			}

			WString GetJsonString(Ptr<JsonNode> node)
			{
				auto stringNode = node.Cast<JsonString>();
				CHECK_ERROR(stringNode, L"JSON string is expected.");
				return stringNode->content.value;
			}

			Ptr<JsonNumber> CreateJsonNumber(vint value)
			{
				auto node = Ptr(new JsonNumber);
				node->content.value = itow(value);
				return node;
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

			bool IsRpcMethod(const WString& rpcMethod, const WString& prefix)
			{
				return rpcMethod.Length() >= prefix.Length() && rpcMethod.Left(prefix.Length()) == prefix;
			}

			WString ReadRpcMethod(Ptr<JsonNode> message)
			{
				auto object = GetJsonObject(message);
				return GetJsonString(GetJsonObjectField(object, WString::Unmanaged(L"rpcMethod")));
			}

			void WriteRpcClientIds(Ptr<JsonNode> message, vint sourceClientId, vint targetClientId)
			{
				auto object = GetJsonObject(message);
				SetJsonObjectField(object, WString::Unmanaged(L"sourceClientId"), CreateJsonNumber(sourceClientId));
				SetJsonObjectField(object, WString::Unmanaged(L"targetClientId"), CreateJsonNumber(targetClientId));
			}

			class RpcJsonObjectEventOpsMock : public Object, public IRpcObjectEventOps
			{
			private:
				RpcDualJsonDispatcherMock*					dispatcher = nullptr;
				IRpcObjectEventOps*							ops = nullptr;

			public:
				RpcJsonObjectEventOpsMock(RpcDualJsonDispatcherMock* _dispatcher, IRpcObjectEventOps* _ops)
					: dispatcher(_dispatcher)
					, ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcJsonObjectEventOpsMock::RpcJsonObjectEventOpsMock(...)#"
					CHECK_ERROR(dispatcher && ops, ERROR_MESSAGE_PREFIX L"Dispatcher and ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				Value InvokeEvent(RpcObjectReference ref, vint eventId, Ptr<IValueArray> arguments)override
				{
					dispatcher->RecordJsonArguments(arguments);
					auto value = ops->InvokeEvent(ref, eventId, arguments);
					dispatcher->RecordJsonValue(value);
					return value;
				}
			};

			class RpcJsonObjectOpsMock : public Object, public IRpcObjectOps
			{
			private:
				RpcDualJsonDispatcherMock*					dispatcher = nullptr;
				IRpcObjectOps*								ops = nullptr;

			public:
				RpcJsonObjectOpsMock(RpcDualJsonDispatcherMock* _dispatcher, IRpcObjectOps* _ops)
					: dispatcher(_dispatcher)
					, ops(_ops)
				{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcJsonObjectOpsMock::RpcJsonObjectOpsMock(...)#"
					CHECK_ERROR(dispatcher && ops, ERROR_MESSAGE_PREFIX L"Dispatcher and ops are required.");
#undef ERROR_MESSAGE_PREFIX
				}

				Value InvokeMethod(RpcObjectReference ref, vint methodId, Ptr<IValueArray> arguments)override
				{
					dispatcher->RecordJsonArguments(arguments);
					auto value = ops->InvokeMethod(ref, methodId, arguments);
					dispatcher->RecordJsonValue(value);
					return value;
				}

				void EndInvokeMethod(vint slot)override
				{
					ops->EndInvokeMethod(slot);
				}

				void ObjectHold(RpcObjectReference ref, vint remoteClientId, bool hold)override
				{
					ops->ObjectHold(ref, remoteClientId, hold);
				}

				void RegisterService(vint typeId, Ptr<reflection::IDescriptable> service)override
				{
					ops->RegisterService(typeId, service);
				}
			};
		}

/***********************************************************************
* RpcDualJsonDispatcherMock
***********************************************************************/

		RpcDualJsonDispatcherMock::RpcDualJsonDispatcherMock(RpcDualLifecycleMock* lc1, RpcDualLifecycleMock* lc2)
			: RpcDualDispatcherMockBase(lc1, lc2)
			, lifecycle1(lc1)
			, lifecycle2(lc2)
		{
		}

		void RpcDualJsonDispatcherMock::RecordJsonValue(const Value& value)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonDispatcherMock::RecordJsonValue(const Value&)#"
			Ptr<JsonNode> node;
			if (value.GetValueType() == Value::SharedPtr)
			{
				if (auto byvalReturnValue = value.GetSharedPtr().Cast<RpcByvalReturnValue>())
				{
					RecordJsonValue(byvalReturnValue->value);
					return;
				}
				node = value.GetSharedPtr().Cast<JsonNode>();
			}
			CHECK_ERROR(node, ERROR_MESSAGE_PREFIX L"RPC JSON value should be a Ptr<JsonNode>.");
			jsonValues.Add(node);
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcDualJsonDispatcherMock::RecordJsonArguments(Ptr<IValueArray> arguments)
		{
			for (vint i = 0; i < arguments->GetCount(); i++)
			{
				RecordJsonValue(arguments->Get(i));
			}
		}

		List<Ptr<JsonNode>>& RpcDualJsonDispatcherMock::JsonValues()
		{
			return jsonValues;
		}

		void RpcDualJsonDispatcherMock::DumpJsonValues(const WString& itemName)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonDispatcherMock::DumpJsonValues(const WString&)#"
			auto folderPath = ::GetJsonValueOutputPath();
			filesystem::Folder folder(folderPath);
			if (!folder.Exists())
			{
				CHECK_ERROR(folder.Create(true), ERROR_MESSAGE_PREFIX L"Failed to create JSON value output folder.");
			}

			stream::FileStream fileStream(folderPath + L"JsonValue_" + itemName + L".ts", stream::FileStream::WriteOnly);
			stream::BomEncoder encoder(stream::BomEncoder::Utf8);
			stream::EncoderStream encoderStream(fileStream, encoder);
			stream::StreamWriter writer(encoderStream);

			writer.WriteString(L"import type { KnownTypeSchema, UnknownTypeSchema } from \"./Serialization_");
			writer.WriteString(itemName);
			writer.WriteLine(L"\";");
			writer.WriteLine(L"");
			writer.WriteLine(L"export const json : (KnownTypeSchema | UnknownTypeSchema)[] = [");
			for (auto [node, index] : indexed(jsonValues))
			{
				writer.WriteString(L"  ");
				JsonPrint(node, writer);
				if (index + 1 < jsonValues.Count())
				{
					writer.WriteString(L",");
				}
				writer.WriteLine(L"");
			}
			writer.WriteLine(L"];");
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcObjectEventOps* RpcDualJsonDispatcherMock::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonDispatcherMock::BroadcastFromClient_ObjectEventOps(vint)#"
			auto ops = RpcDualDispatcherMockBase::BroadcastFromClient_ObjectEventOps(selfClientId);
			Ptr<IRpcObjectEventOps>* opsPtr = nullptr;
			if (lifecycle1 && lifecycle1->GetClientId() == selfClientId) opsPtr = &objectEventOps1;
			if (lifecycle2 && lifecycle2->GetClientId() == selfClientId) opsPtr = &objectEventOps2;
			CHECK_ERROR(opsPtr, ERROR_MESSAGE_PREFIX L"Unknown client id.");
			if (!*opsPtr)
			{
				*opsPtr = Ptr(new RpcJsonObjectEventOpsMock(this, ops));
			}
			return opsPtr->Obj();
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcObjectOps* RpcDualJsonDispatcherMock::SendToClient_ObjectOps(vint targetClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonDispatcherMock::SendToClient_ObjectOps(vint)#"
			auto ops = RpcDualDispatcherMockBase::SendToClient_ObjectOps(targetClientId);
			Ptr<IRpcObjectOps>* opsPtr = nullptr;
			if (lifecycle1 && lifecycle1->GetClientId() == targetClientId) opsPtr = &objectOps1;
			if (lifecycle2 && lifecycle2->GetClientId() == targetClientId) opsPtr = &objectOps2;
			CHECK_ERROR(opsPtr, ERROR_MESSAGE_PREFIX L"Unknown client id.");
			if (!*opsPtr)
			{
				*opsPtr = Ptr(new RpcJsonObjectOpsMock(this, ops));
			}
			return opsPtr->Obj();
#undef ERROR_MESSAGE_PREFIX
		}

/***********************************************************************
* RpcDualJsonMessageBridge
***********************************************************************/

		RpcDualJsonRequestDispatcherMock* RpcDualJsonMessageBridge::GetDispatcher(vint clientId)const
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonMessageBridge::GetDispatcher(vint)const#"
			if (dispatcher1 && dispatcher1->GetLifecycle()->GetClientId() == clientId) return dispatcher1;
			if (dispatcher2 && dispatcher2->GetLifecycle()->GetClientId() == clientId) return dispatcher2;
			CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unknown client id.");
			return nullptr;
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualJsonRequestDispatcherMock* RpcDualJsonMessageBridge::GetOtherDispatcher(RpcDualJsonRequestDispatcherMock* dispatcher)const
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonMessageBridge::GetOtherDispatcher(RpcDualJsonRequestDispatcherMock*)const#"
			if (dispatcher == dispatcher1) return dispatcher2;
			if (dispatcher == dispatcher2) return dispatcher1;
			CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unknown dispatcher.");
			return nullptr;
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualJsonMessageBridge::RpcDualJsonMessageBridge(RpcDualJsonRequestDispatcherMock* _dispatcher1, RpcDualJsonRequestDispatcherMock* _dispatcher2)
			: dispatcher1(_dispatcher1)
			, dispatcher2(_dispatcher2)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonMessageBridge::RpcDualJsonMessageBridge(...)#"
			CHECK_ERROR(dispatcher1 && dispatcher2, ERROR_MESSAGE_PREFIX L"Two dispatchers are required.");
			CHECK_ERROR(dispatcher1 != dispatcher2, ERROR_MESSAGE_PREFIX L"Two different dispatchers are required.");
			dispatcher1->SetBridge(this);
			dispatcher2->SetBridge(this);
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualJsonMessageBridge::~RpcDualJsonMessageBridge()
		{
		}

		void RpcDualJsonMessageBridge::Finalize()
		{
			dispatcher2->Finalize();
			dispatcher1->Finalize();
			services.Clear();
		}

		vint RpcDualJsonMessageBridge::AllocateRequestId()
		{
			return ++nextRequestId;
		}

		bool RpcDualJsonMessageBridge::IsRegisteredService(RpcObjectReference ref)
		{
			return services.Values().Contains(ref);
		}

		void RpcDualJsonMessageBridge::RegisterService(RpcDualJsonRequestDispatcherMock* dispatcher, vint typeId, RpcObjectReference ref)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonMessageBridge::RegisterService(RpcDualJsonRequestDispatcherMock*, vint, RpcObjectReference)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
			auto owner = GetDispatcher(ref.clientId);
			CHECK_ERROR(!services.Keys().Contains(typeId), ERROR_MESSAGE_PREFIX L"Service is already registered.");
			services.Set(typeId, ref);
			owner->GetLifecycle()->LocalObjectHold(ref, dispatcher->GetLifecycle()->GetClientId());
#undef ERROR_MESSAGE_PREFIX
		}

		RpcObjectReference RpcDualJsonMessageBridge::RequestService(vint typeId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonMessageBridge::RequestService(vint)#"
			auto index = services.Keys().IndexOf(typeId);
			CHECK_ERROR(index != -1, ERROR_MESSAGE_PREFIX L"Service is not registered.");
			return services.Values()[index];
#undef ERROR_MESSAGE_PREFIX
		}

		Ptr<JsonNode> RpcDualJsonMessageBridge::OnJsonRequest(RpcDualJsonRequestDispatcherMock* dispatcher, Ptr<JsonNode> message)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonMessageBridge::OnJsonRequest(RpcDualJsonRequestDispatcherMock*, Ptr<JsonNode>)#"
			CHECK_ERROR(dispatcher, ERROR_MESSAGE_PREFIX L"Dispatcher is required.");
			auto receiver = GetOtherDispatcher(dispatcher);
			auto rpcMethod = ReadRpcMethod(message);
			jsonRequests.Add(message);

			Ptr<JsonNode> response;
			if (IsRpcMethod(rpcMethod, WString::Unmanaged(L"IObjectOps_")))
			{
				response = RpcJsonObjectOps::Translate(message, receiver->GetLifecycle()->GetController()->GetObjectOps(), receiver->GetLifecycle());
			}
			else if (IsRpcMethod(rpcMethod, WString::Unmanaged(L"IObjectEventOps_")))
			{
				response = RpcJsonObjectEventOps::Translate(message, receiver->GetLifecycle()->GetController()->GetObjectEventOps());
			}
			else
			{
				CHECK_FAIL(ERROR_MESSAGE_PREFIX L"Unknown JSON RPC method.");
			}

			WriteRpcClientIds(response, receiver->GetLifecycle()->GetClientId(), dispatcher->GetLifecycle()->GetClientId());
			jsonRequests.Add(response);
			return response;
#undef ERROR_MESSAGE_PREFIX
		}

		List<Ptr<JsonNode>>& RpcDualJsonMessageBridge::JsonRequests()
		{
			return jsonRequests;
		}

		void RpcDualJsonMessageBridge::DumpJsonRequests(const WString& itemName)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonMessageBridge::DumpJsonRequests(const WString&)#"
			auto folderPath = ::GetJsonRequestOutputPath();
			filesystem::Folder folder(folderPath);
			if (!folder.Exists())
			{
				CHECK_ERROR(folder.Create(true), ERROR_MESSAGE_PREFIX L"Failed to create JSON request output folder.");
			}

			stream::FileStream fileStream(folderPath + L"JsonRequest_" + itemName + L".ts", stream::FileStream::WriteOnly);
			stream::BomEncoder encoder(stream::BomEncoder::Utf8);
			stream::EncoderStream encoderStream(fileStream, encoder);
			stream::StreamWriter writer(encoderStream);

			writer.WriteString(L"import type { KnownTypeSchema, UnknownTypeSchema } from \"./Serialization_");
			writer.WriteString(itemName);
			writer.WriteLine(L"\";");
			writer.WriteLine(L"import type { Request, Response } from \"../Rpc\";");
			writer.WriteLine(L"");
			writer.WriteLine(L"export const json : (Request<KnownTypeSchema | UnknownTypeSchema> | Response<KnownTypeSchema | UnknownTypeSchema>)[] = [");
			for (auto [node, index] : indexed(jsonRequests))
			{
				writer.WriteString(L"  ");
				JsonPrint(node, writer);
				if (index + 1 < jsonRequests.Count())
				{
					writer.WriteString(L",");
				}
				writer.WriteLine(L"");
			}
			writer.WriteLine(L"];");
#undef ERROR_MESSAGE_PREFIX
		}

/***********************************************************************
* RpcDualJsonRequestDispatcherMock
***********************************************************************/

		RpcDualJsonRequestDispatcherMock::RpcDualJsonRequestDispatcherMock(RpcDualLifecycleMock* _lifecycle)
			: lifecycle(_lifecycle)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonRequestDispatcherMock::RpcDualJsonRequestDispatcherMock(RpcDualLifecycleMock*)#"
			CHECK_ERROR(lifecycle, ERROR_MESSAGE_PREFIX L"Lifecycle is required.");
			lifecycle->SetDispatcher(this);
#undef ERROR_MESSAGE_PREFIX
		}

		RpcDualJsonRequestDispatcherMock::~RpcDualJsonRequestDispatcherMock()
		{
		}

		RpcDualLifecycleMock* RpcDualJsonRequestDispatcherMock::GetLifecycle()const
		{
			return lifecycle;
		}

		void RpcDualJsonRequestDispatcherMock::SetBridge(RpcDualJsonMessageBridge* _bridge)
		{
			bridge = _bridge;
		}

		void RpcDualJsonRequestDispatcherMock::Finalize()
		{
			objectEventOps = nullptr;
			objectOps = nullptr;
			lifecycle->Finalize();
		}

		bool RpcDualJsonRequestDispatcherMock::IsRegisteredService(RpcObjectReference ref)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonRequestDispatcherMock::IsRegisteredService(RpcObjectReference)#"
			CHECK_ERROR(bridge, ERROR_MESSAGE_PREFIX L"Bridge is required.");
			return bridge->IsRegisteredService(ref);
#undef ERROR_MESSAGE_PREFIX
		}

		void RpcDualJsonRequestDispatcherMock::RegisterService(vint typeId, RpcObjectReference ref)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonRequestDispatcherMock::RegisterService(vint, RpcObjectReference)#"
			CHECK_ERROR(bridge, ERROR_MESSAGE_PREFIX L"Bridge is required.");
			bridge->RegisterService(this, typeId, ref);
#undef ERROR_MESSAGE_PREFIX
		}

		RpcObjectReference RpcDualJsonRequestDispatcherMock::RequestService(vint typeId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonRequestDispatcherMock::RequestService(vint)#"
			CHECK_ERROR(bridge, ERROR_MESSAGE_PREFIX L"Bridge is required.");
			return bridge->RequestService(typeId);
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcObjectEventOps* RpcDualJsonRequestDispatcherMock::BroadcastFromClient_ObjectEventOps(vint selfClientId)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonRequestDispatcherMock::BroadcastFromClient_ObjectEventOps(vint)#"
			CHECK_ERROR(selfClientId == lifecycle->GetClientId(), ERROR_MESSAGE_PREFIX L"Unexpected client id.");
			if (!objectEventOps)
			{
				objectEventOps = Ptr(new RpcJsonObjectEventOps(selfClientId, this, lifecycle->GetSerializer()));
			}
			return objectEventOps.Obj();
#undef ERROR_MESSAGE_PREFIX
		}

		IRpcObjectOps* RpcDualJsonRequestDispatcherMock::SendToClient_ObjectOps(vint targetClientId)
		{
			if (!objectOps)
			{
				objectOps = Ptr(new RpcJsonObjectOps(lifecycle->GetClientId(), targetClientId, this, lifecycle->GetSerializer(), lifecycle));
			}
			return objectOps.Obj();
		}

		vint RpcDualJsonRequestDispatcherMock::AllocateRequestId()
		{
			return bridge ? bridge->AllocateRequestId() : ++nextRequestId;
		}

		Ptr<JsonNode> RpcDualJsonRequestDispatcherMock::OnJsonRequest(Ptr<JsonNode> message)
		{
#define ERROR_MESSAGE_PREFIX L"vl::rpc_controller_test::RpcDualJsonRequestDispatcherMock::OnJsonRequest(Ptr<JsonNode>)#"
			CHECK_ERROR(bridge, ERROR_MESSAGE_PREFIX L"Bridge is required.");
			return bridge->OnJsonRequest(this, message);
#undef ERROR_MESSAGE_PREFIX
		}
	}
}
