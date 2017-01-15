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
		::vl::vint8_t i8 = static_cast<::vl::vint8_t>(1);
		::vl::vint16_t i16 = static_cast<::vl::vint16_t>(2);
		::vl::vint32_t i32 = 4;
		::vl::vint64_t i64 = static_cast<::vl::vint64_t>(8);
		::vl::vuint8_t u8 = static_cast<::vl::vuint8_t>(1);
		::vl::vuint16_t u16 = static_cast<::vl::vuint16_t>(2);
		::vl::vuint32_t u32 = static_cast<::vl::vuint32_t>(4);
		::vl::vuint64_t u64 = static_cast<::vl::vuint64_t>(8);
		float f32 = static_cast<float>(1.5);
		double f64 = static_cast<double>(3);
		::vl::WString s = ::vl::WString(L"text", false);
		::test::Point p0 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 0; return __vwsn_temp__; }();
		::test::Point p1 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.y = 0; return __vwsn_temp__; }();
		::test::Point p2 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.y = 0; __vwsn_temp__.x = 0; return __vwsn_temp__; }();
		::test::Point p3 = ::test::Point{};
		::test::Point p4 = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 1; __vwsn_temp__.y = 1; return __vwsn_temp__; }();
		::vl::Nullable<::test::Point> q1 = ::vl::Nullable<::test::Point>(static_cast<::test::Point>(p2));
		::vl::Nullable<::test::Point> q2 = ::vl::Nullable<::test::Point>(static_cast<::test::Point>(p4));
		::vl::Nullable<::test::Point> q3 = ::vl::Nullable<::test::Point>();
		return ((((((((((((((((((((((((((((((((((((((::vl::__vwsn::ToString((i8 < i16)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((i16 > i32))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((i32 == i64))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((u8 <= u16))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((u16 >= u32))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((u32 != u64))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((f32 < f64))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((f32 > f64))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((f32 == f64))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((b == true))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((b != false))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((s == ::vl::WString(L"string", false)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((s != ::vl::WString(L"string", false)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((p0 == p1))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((p1 == p2))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((p2 == p3))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((p3 == p4))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((q1 == q3))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((q2 == q3))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((q3 == q3)));
	}

	OpCompare& OpCompare::Instance()
	{
		return Getvl_workflow_global_OpCompare().instance;
	}
}

