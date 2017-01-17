/***********************************************************************
Generated from ../Resources/Codegen/StaticMethod.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_STATICMETHOD
#define VCZH_WORKFLOW_CPP_GENERATED_STATICMETHOD

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class StaticMethod
	{
	public:

		::vl::Ptr<::test::ObservableValue> Create2(::vl::vint32_t value, const ::vl::WString& name);
		::vl::WString main();

		static StaticMethod& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
