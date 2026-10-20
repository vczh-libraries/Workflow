#include "CppTypes.h"
#include "../SourceCppGen/OrdinaryCastBaseline.h"

using namespace vl;

TEST_FILE
{
	TEST_CASE(L"Ordinary strong null casts preserve the generated C++ baseline")
	{
		auto& instance = ::vl_workflow_global::OrdinaryCastBaseline::Instance();
		TEST_EXCEPTION(instance.StrongRawNull(), Error, [](auto&&) {});
		TEST_EXCEPTION(instance.StrongSharedNull(), Error, [](auto&&) {});
		TEST_EXCEPTION(instance.StrongNullableNull(), Error, [](auto&&) {});
	});
}
