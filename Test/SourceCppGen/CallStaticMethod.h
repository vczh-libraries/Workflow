/***********************************************************************
Generated from ../Resources/Codegen/CallStaticMethod.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CALLSTATICMETHOD
#define VCZH_WORKFLOW_CPP_GENERATED_CALLSTATICMETHOD

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class Calculator;

class Calculator : public virtual ::vl::Object, public ::vl::reflection::Description<Calculator>
{
public:
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class CallStaticMethod
	{
	public:

		::vl::WString main();

		static CallStaticMethod& Instance();
	};
}

#endif
