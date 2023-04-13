/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/NullableCast.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_NULLABLECAST
#define VCZH_WORKFLOW_COMPILER_GENERATED_NULLABLECAST

#include "../Source/CppTypes.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

namespace __vwsn_structs
{
	struct _BoxedIntValue
	{
		::vl::vint value = 0;

		std::strong_ordering operator<=>(const _BoxedIntValue&) const = default;
		bool operator==(const _BoxedIntValue&) const = default;
	};

}
using BoxedIntValue = ::__vwsn_structs::_BoxedIntValue;

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class NullableCast
	{
	public:

		::vl::WString ReadValue(const ::vl::reflection::description::Value& value);
		::vl::WString main();

		static NullableCast& Instance();
	};
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
