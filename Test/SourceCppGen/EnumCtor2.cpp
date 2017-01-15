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
		auto a = ::Seasons::Spring;
		auto b = ::Seasons::Summer;
		auto c = ::vl_workflow_global::EnumCtor2::Instance().Id(::Seasons::Autumn);
		auto d = ::Seasons::Winter;
		return ((((((((((((::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(a)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(b))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(c))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(d))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a | b)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a & ::Seasons::Good)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a & ::Seasons::Bad))));
	}

	EnumCtor2& EnumCtor2::Instance()
	{
		return Getvl_workflow_global_EnumCtor2().instance;
	}
}

