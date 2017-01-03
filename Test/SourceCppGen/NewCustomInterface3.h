/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface3.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE3
#define VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE3

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class IMyInterface3;
class MyClass;

class IMyInterface3 : public virtual ::vl::reflection::description::IValueEnumerable, public ::vl::reflection::Description<IMyInterface3>
{
public:

	virtual ::vl::vint32_t Get100() = 0;
};

class MyClass : public ::vl::Object, public ::vl::reflection::Description<MyClass>
{
public:

	::vl::vint32_t begin = 0;
	::vl::vint32_t end = 0;
	MyClass(::vl::vint32_t _begin, ::vl::vint32_t _end);
	static ::vl::vint32_t Get50();
	::vl::Ptr<::IMyInterface3> CreateMyInterface();
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
