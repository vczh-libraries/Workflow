/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/NullableCast.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "NullableCast.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, NullableCastReflection.h) */
#include "NullableCastReflection.h"
#endif
/* CodePack:EndIgnore() */

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::NullableCast::
#define GLOBAL_NAME ::vl_workflow_global::NullableCast::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::NullableCast::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NullableCast)
	vl_workflow_global::NullableCast instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NullableCast)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString NullableCast::ReadValue(const ::vl::reflection::description::Value& value)
	{
		auto nullable = ::vl::__vwsn::Unbox<::vl::Nullable<::BoxedIntValue>>(value);
		return ((! static_cast<bool>(nullable)) ? ::vl::WString::Unmanaged(L"null") : ::vl::__vwsn::ToString(nullable.Value().value));
	}

	::vl::WString NullableCast::main()
	{
		return ((GLOBAL_NAME ReadValue(::vl::reflection::description::Value()) + ::vl::WString::Unmanaged(L", ")) + GLOBAL_NAME ReadValue(::vl::__vwsn::Box([&](){ ::BoxedIntValue __vwsn_temp__; __vwsn_temp__.value = static_cast<::vl::vint32_t>(0); return __vwsn_temp__; }())));
	}

	NullableCast& NullableCast::Instance()
	{
		return Getvl_workflow_global_NullableCast().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
