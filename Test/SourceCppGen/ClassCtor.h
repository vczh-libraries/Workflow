/***********************************************************************
Generated from ../Resources/Codegen/ClassCtor.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CLASSCTOR
#define VCZH_WORKFLOW_CPP_GENERATED_CLASSCTOR

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class Base;
class Ctor;

class Base : public ::vl::Object, public ::vl::reflection::Description<Base>
{
public:

	::vl::vint32_t x = 0;
	Base();
	Base(::vl::vint32_t _x);
	::vl::vint32_t GetX();
	void SetX(::vl::vint32_t _x);
	::vl::Event<void()> XChanged;
};

class Ctor : public ::Base, public ::test::ObservableValue, public ::vl::reflection::Description<Ctor>
{
public:

	Ctor();
	Ctor(::vl::vint32_t y, bool b);
	Ctor(bool b, ::vl::vint32_t value);
	Ctor(::vl::vint32_t y, ::vl::vint32_t value);
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
