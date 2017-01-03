/***********************************************************************
Generated from ../Resources/Codegen/StructCtor2.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_STRUCTCTOR2
#define VCZH_WORKFLOW_CPP_GENERATED_STRUCTCTOR2

#include "../Source/CppTypes.h"

/***********************************************************************
Structs
***********************************************************************/

struct Point
{
	::vl::vint32_t x = 0;
	::vl::vint32_t y = 0;
	::vl::Nullable<::Point> z;
};

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class StructCtor2
	{
	public:

		::vl::vint32_t Do(::Point p);
		::vl::WString main();

		static StructCtor2& Instance();
	};
}

#endif
