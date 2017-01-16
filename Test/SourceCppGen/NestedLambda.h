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

	virtual ::vl::vint32_t GetA() = 0;
	virtual void SetA(::vl::vint32_t value) = 0;
	::vl::Event<void()> AChanged;
	virtual ::vl::WString Calculate() = 0;
};

class IB : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<IB>
{
public:

	virtual ::vl::vint32_t GetB() = 0;
	virtual void SetB(::vl::vint32_t value) = 0;
	::vl::Event<void()> BChanged;
	virtual ::vl::WString Calculate() = 0;
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
