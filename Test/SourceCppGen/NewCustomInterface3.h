/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface3.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE3
#define VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE3

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class IMyInterface;
class MyClass;

class IMyInterface : public virtual ::vl::reflection::description::IValueEnumerable, public ::vl::reflection::Description<IMyInterface>
{
public:
};

class MyClass : public ::vl::reflection::Description<MyClass>
{
public:
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class NewCustomInterface3
	{
	public:

		::vl::WString main();

		static NewCustomInterface3& Instance();
	};
}

#endif
