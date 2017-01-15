/***********************************************************************
Generated from ../Resources/Codegen/EnumCtor.txt
***********************************************************************/

#include "EnumCtor.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor)
	vl_workflow_global::EnumCtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor)

namespace vl_workflow_global
{
	::test::Seasons EnumCtor::Id(::test::Seasons s)
	{
		return s;
	}

	::vl::WString EnumCtor::main()
	{
		::test::Seasons a = ::test::Seasons::Spring;
		::test::Seasons b = ::test::Seasons::Summer;
		::test::Seasons c = ::vl_workflow_global::EnumCtor::Instance().Id(::test::Seasons::Autumn);
		::test::Seasons d = ::test::Seasons::Winter;
		return (((((((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(a), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(b), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(c), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>(d), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>((a | b)), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>((a & ::test::Seasons::Good)), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vuint64_t>::Serialize(static_cast<::vl::vuint64_t>((a & ::test::Seasons::Bad)), __vwsn_temp__); return __vwsn_temp__; }());
	}

	EnumCtor& EnumCtor::Instance()
	{
		return Getvl_workflow_global_EnumCtor().instance;
	}
}

