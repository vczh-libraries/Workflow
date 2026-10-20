#include "CppTypes.h"
#include "../SourceCppGen/VariantCasts.h"

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
}
