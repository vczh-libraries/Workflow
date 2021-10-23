/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/OpShr.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "OpShr.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::OpShr::
#define GLOBAL_NAME ::vl_workflow_global::OpShr::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::OpShr::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpShr)
	vl_workflow_global::OpShr instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpShr)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString OpShr::main()
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
		auto s = ::vl::WString::Unmanaged(L"text");
		return ((((((((((::vl::__vwsn::ToString((static_cast<::vl::vint32_t>(i8) >> static_cast<::vl::vint32_t>(i16))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString((static_cast<::vl::vint32_t>(i16) >> i32))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString((static_cast<::vl::vint64_t>(i32) >> i64))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString((static_cast<::vl::vuint32_t>(u8) >> static_cast<::vl::vuint32_t>(u16)))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString((static_cast<::vl::vuint32_t>(u16) >> u32))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString((static_cast<::vl::vuint64_t>(u32) >> u64)));
	}

	OpShr& OpShr::Instance()
	{
		return Getvl_workflow_global_OpShr().instance;
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
