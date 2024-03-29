/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/StructCtor.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "StructCtor.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, StructCtorReflection.h) */
#include "StructCtorReflection.h"
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

#define GLOBAL_SYMBOL ::vl_workflow_global::StructCtor::
#define GLOBAL_NAME ::vl_workflow_global::StructCtor::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::StructCtor::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor)
	vl_workflow_global::StructCtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StructCtor)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::vint32_t StructCtor::Do(::test::Point p)
	{
		return (p.x + p.y);
	}

	::vl::WString StructCtor::main()
	{
		auto a = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = static_cast<::vl::vint32_t>(1); __vwsn_temp__.y = static_cast<::vl::vint32_t>(2); return __vwsn_temp__; }();
		auto b = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = static_cast<::vl::vint32_t>(3); __vwsn_temp__.y = static_cast<::vl::vint32_t>(4); return __vwsn_temp__; }();
		return ::vl::__vwsn::ToString(((GLOBAL_NAME Do(a) + GLOBAL_NAME Do(b)) + GLOBAL_NAME Do([&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = static_cast<::vl::vint32_t>(5); __vwsn_temp__.y = static_cast<::vl::vint32_t>(6); return __vwsn_temp__; }())));
	}

	StructCtor& StructCtor::Instance()
	{
		return Getvl_workflow_global_StructCtor().instance;
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
