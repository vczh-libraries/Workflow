/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface2.txt
***********************************************************************/

#include "NewCustomInterface2.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface2)
	vl_workflow_global::NewCustomInterface2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface2)

namespace vl_workflow_global
{
	::vl::WString NewCustomInterface2::main()
	{
		throw 0;
	}

	NewCustomInterface2& NewCustomInterface2::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface2().instance;
	}

	class __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};

	class __vwsnc1_main__IMyInterface2 : public ::vl::Object, public virtual ::IMyInterface2
	{
	public:
		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
		::vl::vint32_t Get50();
		::vl::vint32_t Get100() override;
	};



	::vl::reflection::description::Value __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator()
	{
		throw 0;
	}

	bool __vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator()
	{
		throw 0;
	}


	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_main__IMyInterface2::__vwsnc1_main__IMyInterface2()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc1_main__IMyInterface2::__vwsnc1_main__IMyInterface2()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc1_main__IMyInterface2::__vwsnc1_main__IMyInterface2()
	{
		throw 0;
	}
}

/***********************************************************************
Class (::IMyInterface2)
***********************************************************************/

::vl::vint32_t IMyInterface2::IMyInterface2()
{
	throw 0;
}

