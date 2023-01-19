/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/NewInterface.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_NEWINTERFACE
#define VCZH_WORKFLOW_COMPILER_GENERATED_NEWINTERFACE

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

namespace vl_workflow_global
{
	class __vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable;
	class __vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator;
}

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class NewInterface
	{
	public:

		::vl::WString Show1(const ::vl::collections::LazyList<::vl::vint32_t>& xs);
		::vl::WString Show2(const ::vl::collections::LazyList<::vl::vint32_t>& xs);
		::vl::WString main();

		static NewInterface& Instance();
	};

/***********************************************************************
Closures
***********************************************************************/

	class __vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerable
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;

		__vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end);

		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
	};

	class __vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;
		::vl::reflection::description::IValueEnumerable* __vwsnthis_0;

		__vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::vl::reflection::description::IValueEnumerable* __vwsnctorthis_0);

		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
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
