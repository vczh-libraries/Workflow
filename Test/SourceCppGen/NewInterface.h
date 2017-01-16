/***********************************************************************
Generated from ../Resources/Codegen/NewInterface.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_NEWINTERFACE
#define VCZH_WORKFLOW_CPP_GENERATED_NEWINTERFACE

#include "../Source/CppTypes.h"


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class NewInterface
	{
	public:

		::vl::WString Show1(const ::vl::collections::LazyList<::vl::vint32_t>& xs);
		::vl::WString Show2(const ::vl::collections::LazyList<::vl::vint32_t>& xs);
		::vl::WString main();

		static NewInterface& Instance();
	};
}

#endif
