/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/OpCompareIndexOf.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_OPCOMPAREINDEXOF
#define VCZH_WORKFLOW_COMPILER_GENERATED_OPCOMPAREINDEXOF

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
	struct _IndexPoint
	{
		::vl::vint64_t x = 0;
		::vl::vint64_t y = 0;

		auto operator<=>(const _IndexPoint&) const = default;
	};

}
using IndexPoint = ::__vwsn_structs::_IndexPoint;

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class OpCompareIndexOf
	{
	public:

		::vl::WString main();

		static OpCompareIndexOf& Instance();
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
