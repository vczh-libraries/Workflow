/***********************************************************************
Generated from ../Resources/Codegen/BindFormat.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_BINDFORMAT
#define VCZH_WORKFLOW_CPP_GENERATED_BINDFORMAT

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
	class BindFormat
	{
	public:

		::vl::WString s;

		void Callback(::vl::reflection::description::Value value);
		::vl::WString main();

		static BindFormat& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
