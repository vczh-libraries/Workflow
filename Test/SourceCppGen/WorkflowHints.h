/***********************************************************************
Generated from ../Resources/Codegen/WorkflowHints.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_WORKFLOWHINTS
#define VCZH_WORKFLOW_CPP_GENERATED_WORKFLOWHINTS

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#pragma warning(push)
#pragma warning(disable:4250)
namespace workflow
{
	namespace hints
	{
		class Base;
		class Derived;

		class Base : public ::vl::Object, public ::vl::reflection::Description<Base>
		{
		public:

			Base();
			Base(::vl::vint32_t x);
		};

		class Derived : public ::workflow::hints::Base, public ::vl::reflection::Description<Derived>
		{
		public:

			Derived();
			Derived(::vl::vint32_t x);
		};
#pragma warning(pop)
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
		void ExtraTests();
		::vl::WString main();

		static WorkflowHints& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
