/***********************************************************************
Generated from ../Resources/Codegen/OpPositive.txt
***********************************************************************/

#include "OpPositive.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::OpPositive::
#define GLOBAL_NAME ::vl_workflow_global::OpPositive::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::OpPositive::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpPositive)
	vl_workflow_global::OpPositive instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpPositive)

namespace vl_workflow_global
{
	::vl::WString OpPositive::main()
	{
		auto b = false;
		auto i8 = static_cast<::vl::vint8_t>(1);
		auto i16 = static_cast<::vl::vint16_t>(2);
		auto i32 = 4;
		auto i64 = static_cast<::vl::vint64_t>(8);
		auto u8 = static_cast<::vl::vuint8_t>(1);
		auto u16 = static_cast<::vl::vuint16_t>(2);
		auto u32 = static_cast<::vl::vuint32_t>(4);
		auto u64 = static_cast<::vl::vuint64_t>(8);
		auto f32 = static_cast<float>(1.5);
		auto f64 = static_cast<double>(3);
		auto s = ::vl::WString(L"text", false);
		return ((((((((((((((::vl::__vwsn::ToString((+ i8)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((+ i16))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((+ i32))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((+ i64))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((+ u8))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((+ u16))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((+ u32))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((+ u64)));
	}

	OpPositive& OpPositive::Instance()
	{
		return Getvl_workflow_global_OpPositive().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
