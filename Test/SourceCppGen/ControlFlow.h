/***********************************************************************
Generated from ../Resources/Codegen/ControlFlow.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CONTROLFLOW
#define VCZH_WORKFLOW_CPP_GENERATED_CONTROLFLOW

#include "../Source/CppTypes.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class ControlFlow
	{
	public:

		::vl::vint32_t Sum1(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum2(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum3(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum4(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::vint32_t Sum5(::vl::vint32_t start, ::vl::vint32_t end);
		::vl::WString main();

		static ControlFlow& Instance();
	};
}

#endif
