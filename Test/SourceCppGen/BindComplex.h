/***********************************************************************
Generated from ../Resources/Codegen/BindComplex.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_BINDCOMPLEX
#define VCZH_WORKFLOW_CPP_GENERATED_BINDCOMPLEX

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
	class BindComplex
	{
	public:

		::vl::WString s;

		void Callback(::vl::reflection::description::Value value);
		::vl::WString main();

		static BindComplex& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
