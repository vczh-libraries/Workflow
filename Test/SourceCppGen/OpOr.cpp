/***********************************************************************
Generated from ../Resources/Codegen/OpOr.txt
***********************************************************************/

#include "OpOr.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpOr)
	vl_workflow_global::OpOr instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpOr)

namespace vl_workflow_global
{
	::vl::WString OpOr::main()
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
		return (((((((((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((b || true), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((b || false), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint16_t>::Serialize(static_cast<::vl::vint16_t>(static_cast<::vl::vint16_t>(i8) | i16), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize((static_cast<::vl::vint32_t>(i16) | i32), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint64_t>::Serialize((static_cast<::vl::vint64_t>(i32) | i64), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint16_t>::Serialize(static_cast<::vl::vuint16_t>(static_cast<::vl::vuint16_t>(u8) | u16), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint32_t>::Serialize((static_cast<::vl::vuint32_t>(u16) | u32), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize((static_cast<::vl::vuint64_t>(u32) | u64), __vwsn_temp__); return __vwsn_temp__; }());
	}

	OpOr& OpOr::Instance()
	{
		return Getvl_workflow_global_OpOr().instance;
	}
}

