/***********************************************************************
Generated from ../Resources/Codegen/ForEach.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_FOREACH
#define VCZH_WORKFLOW_CPP_GENERATED_FOREACH

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class ForEach
	{
	public:

		::vl::WString Show1(::vl::Ptr<::vl::reflection::description::IValueList> xs);
		::vl::WString Show2(::vl::Ptr<::vl::reflection::description::IValueList> xs);
		::vl::WString main();

		static ForEach& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
