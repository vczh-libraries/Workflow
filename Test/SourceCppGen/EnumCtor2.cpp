/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/EnumCtor2.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "EnumCtor2.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, EnumCtor2Reflection.h) */
#include "EnumCtor2Reflection.h"
#endif
/* CodePack:EndIgnore() */

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::EnumCtor2::
#define GLOBAL_NAME ::vl_workflow_global::EnumCtor2::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::EnumCtor2::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor2)
	vl_workflow_global::EnumCtor2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor2)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::Seasons EnumCtor2::Id(::Seasons s)
	{
		return s;
	}

	::vl::WString EnumCtor2::main()
	{
		auto a = ::Seasons::Spring;
		auto b = ::Seasons::Summer;
		auto c = GLOBAL_NAME Id(::Seasons::Autumn);
		auto d = ::Seasons::Winter;
		return ((((((((((((::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(a)) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(b))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(c))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(d))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a | b)))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a & ::Seasons::Good)))) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a & ::Seasons::Bad))));
	}

	EnumCtor2& EnumCtor2::Instance()
	{
		return Getvl_workflow_global_EnumCtor2().instance;
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
