#include "CppTypes.h"
#include "../SourceCppGen/VariantCasts.h"
#include "../SourceCppGenRpc/RpcPrimitiveTypes.h"

using namespace vl;
using namespace vl::reflection::description;

TEST_FILE
{
	TEST_CASE(L"An escaped variant cannot be extracted by an ordinary native cast")
	{
		auto& instance = ::vl_workflow_global::VariantCasts::Instance();
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		TEST_EXCEPTION(instance.EscapedPayload(), ArgumentTypeMismtatchException, [](auto&&) {});
#else
		TEST_EXCEPTION(instance.EscapedPayload(), Error, [](auto&&) {});
#endif
	});

	TEST_CASE(L"RPC JSON rejects escaped variants directly and in dynamic collections")
	{
		auto& instance = ::vl_workflow_global::VariantCasts::Instance();
		for (vint mode = 0; mode < 3; mode++)
		{
			TEST_EXCEPTION(instance.SerializeEscapedVariant(mode), Error, [](const Error& exception)
			{
				TEST_ASSERT(WString(exception.Description()) == L"Unsupported RPC JSON serialization value.");
			});
		}
	});

	TEST_CASE(L"Generated RPC serializers reject variant boxes before choosing a schema")
	{
		Variant<vint, WString> variant(vint(1));
		auto boxed = BoxParameter(variant);
		auto list = IValueList::Create();
		list->Add(boxed);
		auto dictionary = IValueDictionary::Create();
		dictionary->Set(BoxValue(WString(L"value")), boxed);
		Value inputs[] = { boxed, BoxParameter(list), BoxParameter(dictionary) };
		auto& instance = ::vl_workflow_global::Rpc_PrimitiveTypes::Instance();
		for (auto&& input : inputs)
		{
			TEST_EXCEPTION(instance.rpcjson_Serialize(input), Error, [](const Error& exception)
			{
				TEST_ASSERT(WString(exception.Description()) == L"Unsupported RPC JSON serialization value.");
			});
		}
	});
}
