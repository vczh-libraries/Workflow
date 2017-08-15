/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/OpCompare.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_OPCOMPARE
#define VCZH_WORKFLOW_COMPILER_GENERATED_OPCOMPARE

#include "../Source/CppTypes.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

enum class MyEnum : vl::vuint64_t
{
	Item = 0UL,
};
inline MyEnum operator& (MyEnum a, MyEnum b) { return static_cast<MyEnum>(static_cast<::vl::vuint64_t>(a) & static_cast<::vl::vuint64_t>(b)); }
inline MyEnum operator| (MyEnum a, MyEnum b) { return static_cast<MyEnum>(static_cast<::vl::vuint64_t>(a) | static_cast<::vl::vuint64_t>(b)); }

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class OpCompare
	{
	public:

		::vl::WString main();

		static OpCompare& Instance();
	};
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
