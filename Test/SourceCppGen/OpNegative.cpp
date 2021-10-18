/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/OpNegative.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "OpNegative.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::OpNegative::
#define GLOBAL_NAME ::vl_workflow_global::OpNegative::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::OpNegative::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNegative)
	vl_workflow_global::OpNegative instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNegative)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString OpNegative::main()
	{
		auto b = false;
		auto i8 = static_cast<::vl::vint8_t>(1);
		auto i16 = static_cast<::vl::vint16_t>(2);
		auto i32 = static_cast<::vl::vint32_t>(4);
		auto i64 = static_cast<::vl::vint64_t>(8L);
		auto u8 = static_cast<::vl::vuint8_t>(1);
		auto u16 = static_cast<::vl::vuint16_t>(2);
		auto u32 = static_cast<::vl::vuint32_t>(4U);
		auto u64 = static_cast<::vl::vuint64_t>(8UL);
		auto f32 = static_cast<float>(1.5f);
		auto f64 = static_cast<double>(3);
		auto s = ::vl::WString(L"text", false);
		return ((((((::vl::__vwsn::ToString((- i8)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((- i16))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((- i32))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((- i64)));
	}

	OpNegative& OpNegative::Instance()
	{
		return Getvl_workflow_global_OpNegative().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
