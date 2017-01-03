/***********************************************************************
Generated from ../Resources/Codegen/OrderedLambda.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_ORDEREDLAMBDA
#define VCZH_WORKFLOW_CPP_GENERATED_ORDEREDLAMBDA

#include "../Source/CppTypes.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class OrderedLambda
	{
	public:

		::vl::vint32_t Apply(::vl::Func<::vl::vint32_t(::vl::vint32_t)> f, ::vl::vint32_t arg);
		::vl::Func<::vl::vint32_t(::vl::vint32_t)> Adder(::vl::vint32_t x);
		::vl::WString main();

		static OrderedLambda& Instance();
	};
}

#endif
