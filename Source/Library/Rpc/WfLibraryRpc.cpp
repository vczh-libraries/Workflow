#include "WfLibraryRpc.h"

namespace vl
{
	namespace rpc_controller
	{
		using namespace reflection::description;

		Value BoxRpcObjectReference(RpcObjectReference ref)
		{
			return Value::From(Ptr(new IValueType::TypedBox<RpcObjectReference>(ref)), nullptr);
		}

		Value BoxRpcException(RpcException exception)
		{
			return Value::From(Ptr(new IValueType::TypedBox<RpcException>(exception)), nullptr);
		}

		RpcEventExceptionMap CreateRpcEventExceptionMap()
		{
			return IValueDictionary::Create();
		}

		void MergeRpcEventExceptionMap(RpcEventExceptionMap target, RpcEventExceptionMap source)
		{
			if (!source) return;
			auto keys = source->GetKeys();
			for (vint i = 0; i < keys->GetCount(); i++)
			{
				auto key = keys->Get(i);
				target->Set(key, source->Get(key));
			}
		}

		Value BoxRpcEventExceptionMap(RpcEventExceptionMap exceptions)
		{
			return exceptions ? BoxValue(exceptions) : Value();
		}

		RpcEventExceptionMap UnboxRpcEventExceptionMap(const Value& value)
		{
			return value.IsNull() ? nullptr : UnboxValue<RpcEventExceptionMap>(value);
		}
	}
}
