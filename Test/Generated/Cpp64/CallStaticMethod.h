/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CallStaticMethod.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_CALLSTATICMETHOD
#define VCZH_WORKFLOW_COMPILER_GENERATED_CALLSTATICMETHOD

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

class Calculator;

class Calculator : public ::vl::Object, public ::vl::reflection::Description<Calculator>
{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
	friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<Calculator>;
#endif
public:
	class Internal;

	class Internal : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<Internal>
	{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<Internal>;
#endif
	public:
		static ::vl::vint64_t Sum(::vl::vint64_t begin, ::vl::vint64_t end);
	};
	static ::vl::vint64_t Sum2(::vl::vint64_t begin, ::vl::vint64_t end, ::vl::vint64_t sum);
	Calculator();
};

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

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
