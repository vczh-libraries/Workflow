/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/ClassCtor.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_CLASSCTOR
#define VCZH_WORKFLOW_COMPILER_GENERATED_CLASSCTOR

#include "../Source/CppTypes.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

class Base;
class Ctor;

class Base : public ::vl::Object, public ::vl::reflection::Description<Base>
{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
	friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<Base>;
#endif
public:
	::vl::vint32_t x;
	Base();
	Base(::vl::vint32_t _x);
	::vl::vint32_t GetX();
	void SetX(::vl::vint32_t _x);
	::vl::Event<void()> XChanged;
};

class Ctor : public ::Base, public ::test::ObservableValue, public ::vl::reflection::Description<Ctor>
{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
	friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<Ctor>;
#endif
public:
	Ctor();
	Ctor(::vl::vint32_t y, bool b);
	Ctor(bool b, ::vl::vint32_t value);
	Ctor(::vl::vint32_t y, ::vl::vint32_t value);
};

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

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
