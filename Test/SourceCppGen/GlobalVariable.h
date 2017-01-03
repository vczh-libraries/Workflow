/***********************************************************************
Generated from ../Resources/Codegen/GlobalVariable.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_GLOBALVARIABLE
#define VCZH_WORKFLOW_CPP_GENERATED_GLOBALVARIABLE

#include "../Source/CppTypes.h"


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class GlobalVariable
	{
	public:

		::vl::vint32_t x = 0;

		::vl::vint32_t Add(::vl::vint32_t y);
		::vl::WString main();

		static GlobalVariable& Instance();
	};
}

#endif
