/***********************************************************************
Generated from ../Resources/Codegen/NewInterface.txt
***********************************************************************/

#include "NewInterface.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)
	vl_workflow_global::NewInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)

namespace vl_workflow_global
{
	::vl::WString NewInterface::Show1(const ::vl::collections::LazyList<::vl::vint32_t>& xs)
	{
		throw 0;
	}

	::vl::WString NewInterface::Show2(const ::vl::collections::LazyList<::vl::vint32_t>& xs)
	{
		throw 0;
	}

	::vl::WString NewInterface::main()
	{
		throw 0;
	}

	NewInterface& NewInterface::Instance()
	{
		return Getvl_workflow_global_NewInterface().instance;
	}

	class __vwsnc1_main__vl_reflection_description_IValueEnumerable : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerable
	{
	public:
		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
	};

	class __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};


	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_main__vl_reflection_description_IValueEnumerable::CreateEnumerator()
	{
		throw 0;
	}



	::vl::reflection::description::Value __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetCurrent()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetIndex()
	{
		throw 0;
	}

	bool __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::Next()
	{
		throw 0;
	}
}

