/***********************************************************************
Generated from ../Resources/Codegen/BindSimple.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_BINDSIMPLE
#define VCZH_WORKFLOW_CPP_GENERATED_BINDSIMPLE

#include "../Source/CppTypes.h"


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class BindSimple
	{
	public:

		::vl::WString s;

		void Callback(::vl::reflection::description::Value value);
		::vl::WString main();

		static BindSimple& Instance();
	};
}

#endif
