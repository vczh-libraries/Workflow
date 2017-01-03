/***********************************************************************
Generated from ../Resources/Codegen/ClassCtor.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CLASSCTOR
#define VCZH_WORKFLOW_CPP_GENERATED_CLASSCTOR

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class Y;
class A;

class Y : public ::vl::Object, public ::vl::reflection::Description<Y>
{
public:

	::vl::vint32_t x = 0;
	Y();
	Y(::vl::vint32_t _x);
};

class A : public ::Y, public ::test::ObservableValue, public ::vl::reflection::Description<A>
{
public:

	A();
	A(::vl::vint32_t y, bool b);
	A(bool b, ::vl::vint32_t value);
	A(::vl::vint32_t y, ::vl::vint32_t value);
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class ClassCtor
	{
	public:

		::vl::WString s;

		::vl::WString main();

		static ClassCtor& Instance();
	};
}

#endif
