/***********************************************************************
Generated from ../Resources/Codegen/ClassMethod.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CLASSMETHOD
#define VCZH_WORKFLOW_CPP_GENERATED_CLASSMETHOD

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class A;

class A : public ::vl::Object, public ::vl::reflection::Description<A>
{
public:

	::vl::vint32_t x = 0;
	A();
	A(::vl::vint32_t _x);
	::vl::vint32_t GetX();
	void SetX(::vl::vint32_t _x);
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class ClassMethod
	{
	public:

		::vl::WString s;

		::vl::WString main();

		static ClassMethod& Instance();
	};
}

#endif
