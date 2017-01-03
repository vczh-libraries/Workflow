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

	::vl::vint32_t GetA();
	void SetA(::vl::vint32_t value);
	::vl::Event<void()> AChanged;
	::vl::WString Calculate();
};

class IB : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<IB>
{
public:

	::vl::vint32_t GetB();
	void SetB(::vl::vint32_t value);
	::vl::Event<void()> BChanged;
	::vl::WString Calculate();
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
