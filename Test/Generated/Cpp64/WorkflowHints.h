/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/WorkflowHints.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWHINTS
#define VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWHINTS

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

namespace workflow
{
	namespace hints
	{
		class Base;
		class Derived;

		class Base : public ::vl::Object, public ::vl::reflection::Description<Base>
		{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
			friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<Base>;
#endif
		public:
			Base();
			Base(::vl::vint64_t x);
		};

		class Derived : public ::workflow::hints::Base, public ::vl::reflection::Description<Derived>
		{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
			friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<Derived>;
#endif
		public:
			Derived();
			Derived(::vl::vint64_t x);
		};

	}
}
/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class WorkflowHints
	{
	public:

		void Ensure(bool condition);
		void Ensure(bool a, bool b);
		void Ensure(::vl::vint64_t a, ::vl::vint64_t b);
		void ExtraTests();
		::vl::WString main();

		static WorkflowHints& Instance();
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
