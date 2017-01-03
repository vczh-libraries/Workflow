/***********************************************************************
Generated from ../Resources/Codegen/CallStaticMethod.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CALLSTATICMETHOD
#define VCZH_WORKFLOW_CPP_GENERATED_CALLSTATICMETHOD

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class Calculator;

class Calculator : public ::vl::Object, public ::vl::reflection::Description<Calculator>
{
public:
	class Internal;

	class Internal : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<Internal>
	{
	public:

	};

	Calculator();
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
