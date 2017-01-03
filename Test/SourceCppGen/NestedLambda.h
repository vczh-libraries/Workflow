/***********************************************************************
Generated from ../Resources/Codegen/NestedLambda.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_NESTEDLAMBDA
#define VCZH_WORKFLOW_CPP_GENERATED_NESTEDLAMBDA

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class IA;
class IB;

class IA : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<IA>
{
public:
};

class IB : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<IB>
{
public:
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class NestedLambda
	{
	public:

		::vl::WString main();

		static NestedLambda& Instance();
	};
}

#endif
