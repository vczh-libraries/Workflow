/***********************************************************************
Generated from ../Resources/Codegen/FailedThen.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_FAILEDTHEN
#define VCZH_WORKFLOW_CPP_GENERATED_FAILEDTHEN

#include "../Source/CppTypes.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class FailedThen
	{
	public:

		::vl::vint32_t GetValue(::vl::Ptr<::test::ObservableValue> value, ::vl::vint32_t increase, ::vl::vint32_t defaultValue);
		::vl::WString main();

		static FailedThen& Instance();
	};
}

#endif
