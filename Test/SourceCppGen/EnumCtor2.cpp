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
		return ((((((((((((static_cast<::vl::vuint64_t>(static_cast<::Seasons>(a)) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint64_t>(static_cast<::Seasons>(b))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint64_t>(static_cast<::Seasons>(c))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint64_t>(static_cast<::Seasons>(d))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint64_t>(static_cast<::Seasons>((a | b)))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint64_t>(static_cast<::Seasons>((a & ::Seasons::Good)))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint64_t>(static_cast<::Seasons>((a & ::Seasons::Bad))));
	}

	EnumCtor2& EnumCtor2::Instance()
	{
		return Getvl_workflow_global_EnumCtor2().instance;
	}
}

