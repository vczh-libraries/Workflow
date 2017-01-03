/***********************************************************************
Generated from ../Resources/Codegen/BindComplex.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_BINDCOMPLEX
#define VCZH_WORKFLOW_CPP_GENERATED_BINDCOMPLEX

#include "../Source/CppTypes.h"

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

#endif
