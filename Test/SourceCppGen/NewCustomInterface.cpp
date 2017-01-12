/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface.txt
***********************************************************************/

#include "NewCustomInterface.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface)
	vl_workflow_global::NewCustomInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface)

namespace vl_workflow_global
{
	::vl::WString NewCustomInterface::main()
	{
		throw 0;
	}

	NewCustomInterface& NewCustomInterface::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface().instance;
	}

	class __vwsnc1_NewCustomInterface_main__IMyInterface : public ::vl::Object, public virtual ::IMyInterface
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;

		__vwsnc1_NewCustomInterface_main__IMyInterface(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end);

		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
		::vl::vint32_t Get100() override;
	};

	class __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;
		::IMyInterface* __vwsnthis_0;

		__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::IMyInterface* __vwsnctorthis_0);

		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};

	__vwsnc1_NewCustomInterface_main__IMyInterface::__vwsnc1_NewCustomInterface_main__IMyInterface(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
	{
	}

	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_NewCustomInterface_main__IMyInterface::CreateEnumerator()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc1_NewCustomInterface_main__IMyInterface::Get100()
	{
		throw 0;
	}

	__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::IMyInterface* __vwsnctorthis_0)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
		this->index = (- 1);
	}

	::vl::reflection::description::Value __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetCurrent()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetIndex()
	{
		throw 0;
	}

	bool __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::Next()
	{
		throw 0;
	}

}

/***********************************************************************
Class (::IMyInterface)
***********************************************************************/

::vl::vint32_t IMyInterface::Get50()
{
	throw 0;
}

