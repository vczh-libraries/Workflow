/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface3.txt
***********************************************************************/

#include "NewCustomInterface3.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface3)
	vl_workflow_global::NewCustomInterface3 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface3)

namespace vl_workflow_global
{
	::vl::WString NewCustomInterface3::main()
	{
		::vl::Ptr<::MyClass> x = ::vl::Ptr<::MyClass>(new ::MyClass(1, 5));
		::vl::Ptr<::IMyInterface3> xs = ::vl::__vwsn::This(x)->CreateMyInterface();
		return ((((((::vl::WString(L"[", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(xs)->Get100(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"][", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::test::CustomInterfaceProcessor::Sum(xs), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"][", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::test::CustomInterfaceProcessor::Sum(xs), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"]", false));
	}

	NewCustomInterface3& NewCustomInterface3::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface3().instance;
	}

	class __vwsnc1_NewCustomInterface3_MyClass_CreateMyInterface__IMyInterface3 : public ::vl::Object, public virtual ::IMyInterface3
	{
	public:
		::MyClass* __vwsnthis_0;

		__vwsnc1_NewCustomInterface3_MyClass_CreateMyInterface__IMyInterface3(::MyClass* __vwsnctorthis_0);

		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
		::vl::vint32_t Get100() override;
	};

	class __vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::IMyInterface3* __vwsnthis_0;
		::MyClass* __vwsnthis_1;

		__vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator(::IMyInterface3* __vwsnctorthis_0, ::MyClass* __vwsnctorthis_1);

		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};

	__vwsnc1_NewCustomInterface3_MyClass_CreateMyInterface__IMyInterface3::__vwsnc1_NewCustomInterface3_MyClass_CreateMyInterface__IMyInterface3(::MyClass* __vwsnctorthis_0)
		:__vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_NewCustomInterface3_MyClass_CreateMyInterface__IMyInterface3::CreateEnumerator()
	{
		return ::vl::Ptr<::vl::reflection::description::IValueEnumerator>(new ::vl_workflow_global::__vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator(this, __vwsnthis_0));
	}

	::vl::vint32_t __vwsnc1_NewCustomInterface3_MyClass_CreateMyInterface__IMyInterface3::Get100()
	{
		return (::MyClass::Get50() * 2);
	}

	__vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator(::IMyInterface3* __vwsnctorthis_0, ::MyClass* __vwsnctorthis_1)
		:__vwsnthis_0(__vwsnctorthis_0)
		, __vwsnthis_1(__vwsnctorthis_1)
	{
		this->index = (- 1);
	}

	::vl::reflection::description::Value __vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetCurrent()
	{
		return ::vl::reflection::description::BoxValue<::vl::vint32_t>((::vl::__vwsn::This(__vwsnthis_1)->begin + index));
	}

	::vl::vint32_t __vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetIndex()
	{
		return index;
	}

	bool __vwsnc2_NewCustomInterface3_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator::Next()
	{
		if ((index == (::vl::__vwsn::This(__vwsnthis_1)->end - ::vl::__vwsn::This(__vwsnthis_1)->begin)))
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
Class (::IMyInterface3)
***********************************************************************/

/***********************************************************************
Class (::MyClass)
***********************************************************************/

MyClass::MyClass(::vl::vint32_t _begin, ::vl::vint32_t _end)
{
	(::vl::__vwsn::This(this)->begin = _begin);
	(::vl::__vwsn::This(this)->end = _end);
}

::vl::vint32_t MyClass::Get50()
{
	return 50;
}

::vl::Ptr<::IMyInterface3> MyClass::CreateMyInterface()
{
	return ::vl::Ptr<::IMyInterface3>(new ::vl_workflow_global::__vwsnc1_NewCustomInterface3_MyClass_CreateMyInterface__IMyInterface3(this));
}

