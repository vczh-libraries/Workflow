/***********************************************************************
Generated from ../Resources/Codegen/BindFormat.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_BINDFORMAT
#define VCZH_WORKFLOW_CPP_GENERATED_BINDFORMAT

#include "../Source/CppTypes.h"

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

#endif
