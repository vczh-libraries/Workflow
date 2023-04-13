/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/StructCtor2.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_STRUCTCTOR2
#define VCZH_WORKFLOW_COMPILER_GENERATED_STRUCTCTOR2

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
	struct _Point
	{
		::vl::vint32_t x = 0;
		::vl::vint32_t y = 0;
		::vl::Nullable<::vl::vint32_t> z;

		std::strong_ordering operator<=>(const _Point&) const = default;
		bool operator==(const _Point&) const = default;
	};

}
using Point = ::__vwsn_structs::_Point;

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class StructCtor2
	{
	public:

		::vl::vint32_t Do(::Point p);
		::vl::WString main();

		static StructCtor2& Instance();
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
