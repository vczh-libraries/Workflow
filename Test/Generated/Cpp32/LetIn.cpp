/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/LetIn.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "LetIn.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::LetIn::
#define GLOBAL_NAME ::vl_workflow_global::LetIn::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::LetIn::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_LetIn)
	vl_workflow_global::LetIn instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_LetIn)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString LetIn::main()
	{
		return [&](auto x, auto y){ return ::vl::__vwsn::ToString((x + y)); }(static_cast<::vl::vint32_t>(10), static_cast<::vl::vint32_t>(20));
	}

	LetIn& LetIn::Instance()
	{
		return Getvl_workflow_global_LetIn().instance;
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
