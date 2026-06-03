#include "RpcDualJsonDispatcherMock.h"
#include "../../Source/Library/Rpc/WfLibraryRpcWrappers.h"

extern vl::WString GetJsonValueOutputPath();

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
	}
}
