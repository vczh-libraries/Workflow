/***********************************************************************
Generated from ../Resources/Codegen/StructCtor2.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_STRUCTCTOR2
#define VCZH_WORKFLOW_CPP_GENERATED_STRUCTCTOR2

#include "../Source/CppTypes.h"

struct Point
{
	::vl::vint32_t x = 0;
	::vl::vint32_t y = 0;
	::vl::Nullable<::Point> z;
};
inline bool operator== (const Point& a, const Point& b)
{
	if (a.x != b.x) return false;
	if (a.y != b.y) return false;
	if (a.z != b.z) return false;
	return true;
}
inline bool operator!= (const Point& a, const Point& b)
{
	if (a.x != b.x) return true;
	if (a.y != b.y) return true;
	if (a.z != b.z) return true;
	return false;
}


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
