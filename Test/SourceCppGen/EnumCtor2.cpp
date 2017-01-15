/***********************************************************************
Generated from ../Resources/Codegen/EnumCtor2.txt
***********************************************************************/

#include "EnumCtor2.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor2)
	vl_workflow_global::EnumCtor2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor2)

namespace vl_workflow_global
{
	::Seasons EnumCtor2::Id(::Seasons s)
	{
		return s;
	}

	::vl::WString EnumCtor2::main()
	{
		::Seasons a = ::Seasons::Spring;
		::Seasons b = ::Seasons::Summer;
		::Seasons c = ::vl_workflow_global::EnumCtor2::Instance().Id(::Seasons::Autumn);
		::Seasons d = ::Seasons::Winter;
		return (((((((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(a), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(b), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(c), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(d), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>((a | b)), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>((a & ::Seasons::Good)), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>((a & ::Seasons::Bad)), __vwsn_temp__); return __vwsn_temp__; }());
	}

	EnumCtor2& EnumCtor2::Instance()
	{
		return Getvl_workflow_global_EnumCtor2().instance;
	}
}

