/***********************************************************************
Generated from ../Resources/Codegen/OpCompare.txt
***********************************************************************/

#include "OpCompare.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompare)
	vl_workflow_global::OpCompare instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompare)

namespace vl_workflow_global
{
	::vl::WString OpCompare::main()
	{
		bool b = false;
		::vl::vint8_t i8 = static_cast<::vl::vint32_t>(1);
		::vl::vint16_t i16 = static_cast<::vl::vint32_t>(2);
		::vl::vint32_t i32 = 4;
		::vl::vint64_t i64 = static_cast<::vl::vint32_t>(8);
		::vl::vuint8_t u8 = static_cast<::vl::vint32_t>(1);
		::vl::vuint16_t u16 = static_cast<::vl::vint32_t>(2);
		::vl::vuint32_t u32 = static_cast<::vl::vint32_t>(4);
		::vl::vuint64_t u64 = static_cast<::vl::vint32_t>(8);
		float f32 = static_cast<double>(1.5);
		double f64 = static_cast<::vl::vint32_t>(3);
		::vl::WString s = ::vl::WString(L"text", false);
		::test::Point p0 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 0; return __vwsn_temp__; }();
		::test::Point p1 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.y = 0; return __vwsn_temp__; }();
		::test::Point p2 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.y = 0; __vwsn_temp__.x = 0; return __vwsn_temp__; }();
		::test::Point p3 = ::test::Point{};
		::test::Point p4 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 1; __vwsn_temp__.y = 1; return __vwsn_temp__; }();
		::vl::Nullable<::test::Point> q1 = static_cast<::test::Point>(p2);
		::vl::Nullable<::test::Point> q2 = static_cast<::test::Point>(p4);
		::vl::Nullable<::test::Point> q3 = nullptr;
		return ((((((((((((((((((((((((((((((((((((((static_cast<bool>((i8 < i16)) + ::vl::WString(L", ", false)) + static_cast<bool>((i16 > i32))) + ::vl::WString(L", ", false)) + static_cast<bool>((i32 == i64))) + ::vl::WString(L", ", false)) + static_cast<bool>((u8 <= u16))) + ::vl::WString(L", ", false)) + static_cast<bool>((u16 >= u32))) + ::vl::WString(L", ", false)) + static_cast<bool>((u32 != u64))) + ::vl::WString(L", ", false)) + static_cast<bool>((f32 < f64))) + ::vl::WString(L", ", false)) + static_cast<bool>((f32 > f64))) + ::vl::WString(L", ", false)) + static_cast<bool>((f32 == f64))) + ::vl::WString(L", ", false)) + static_cast<bool>((b == true))) + ::vl::WString(L", ", false)) + static_cast<bool>((b != false))) + ::vl::WString(L", ", false)) + static_cast<bool>((s == ::vl::WString(L"string", false)))) + ::vl::WString(L", ", false)) + static_cast<bool>((s != ::vl::WString(L"string", false)))) + ::vl::WString(L", ", false)) + static_cast<bool>((p0 == p1))) + ::vl::WString(L", ", false)) + static_cast<bool>((p1 == p2))) + ::vl::WString(L", ", false)) + static_cast<bool>((p2 == p3))) + ::vl::WString(L", ", false)) + static_cast<bool>((p3 == p4))) + ::vl::WString(L", ", false)) + static_cast<bool>((q1 == q3))) + ::vl::WString(L", ", false)) + static_cast<bool>((q2 == q3))) + ::vl::WString(L", ", false)) + static_cast<bool>((q3 == q3)));
	}

	OpCompare& OpCompare::Instance()
	{
		return Getvl_workflow_global_OpCompare().instance;
	}
}

