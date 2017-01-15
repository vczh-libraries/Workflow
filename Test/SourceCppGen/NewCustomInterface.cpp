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

	::vl::WString NewCustomInterface::main()
	{
		::vl::vint32_t begin = 1;
		::vl::vint32_t end = 5;
		::vl::Ptr<::IMyInterface> xs = ::vl::Ptr<::IMyInterface>(new ::vl_workflow_global::__vwsnc1_NewCustomInterface_main__IMyInterface(begin, end));
		return ((((((::vl::WString(L"[", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(xs)->Get100(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"][", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::test::CustomInterfaceProcessor::Sum(xs.Cast<::vl::reflection::description::IValueEnumerable>()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"][", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::test::CustomInterfaceProcessor::Sum(xs.Cast<::vl::reflection::description::IValueEnumerable>()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"]", false));
	}

	NewCustomInterface& NewCustomInterface::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface().instance;
	}

	__vwsnc1_NewCustomInterface_main__IMyInterface::__vwsnc1_NewCustomInterface_main__IMyInterface(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
	{
	}

	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_NewCustomInterface_main__IMyInterface::CreateEnumerator()
	{
		return ::vl::Ptr<::vl::reflection::description::IValueEnumerator>(new ::vl_workflow_global::__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::__vwsn::This(this)->begin, ::vl::__vwsn::This(this)->end, this));
	}

	::vl::vint32_t __vwsnc1_NewCustomInterface_main__IMyInterface::Get100()
	{
		return (::IMyInterface::Get50() * 2);
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
		return ::vl::reflection::description::BoxValue<::vl::vint32_t>((::vl::__vwsn::This(this)->begin + index));
	}

	::vl::vint32_t __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetIndex()
	{
		return index;
	}

	bool __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::Next()
	{
		if ((index == (::vl::__vwsn::This(this)->end - ::vl::__vwsn::This(this)->begin)))
		{
			return false;
		}
		else
		{
			(index = (index + 1));
			return true;
		}
	}

}

/***********************************************************************
Class (::IMyInterface)
***********************************************************************/

::vl::vint32_t IMyInterface::Get50()
{
	return 50;
}

