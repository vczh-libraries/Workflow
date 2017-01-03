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
		throw 0;
	}

	NewCustomInterface3& NewCustomInterface3::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface3().instance;
	}

	class __vwsnc2_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};

	class __vwsnc1_MyClass_CreateMyInterface__IMyInterface3 : public ::vl::Object, public virtual ::IMyInterface3
	{
	public:
		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
		::vl::vint32_t Get100() override;
	};



	::vl::reflection::description::Value __vwsnc2_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc2_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator()
	{
		throw 0;
	}

	bool __vwsnc2_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_MyClass_CreateMyInterface_CreateEnumerator__vl_reflection_description_IValueEnumerator()
	{
		throw 0;
	}


	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_MyClass_CreateMyInterface__IMyInterface3::__vwsnc1_MyClass_CreateMyInterface__IMyInterface3()
	{
		throw 0;
	}

	::vl::vint32_t __vwsnc1_MyClass_CreateMyInterface__IMyInterface3::__vwsnc1_MyClass_CreateMyInterface__IMyInterface3()
	{
		throw 0;
	}
}

/***********************************************************************
Class (::IMyInterface3)
***********************************************************************/

::vl::vint32_t IMyInterface3::IMyInterface3()
{
	throw 0;
}

/***********************************************************************
Class (::MyClass)
***********************************************************************/

MyClass::MyClass(::vl::vint32_t _begin, ::vl::vint32_t _end)
{
	throw 0;
}

::vl::vint32_t MyClass::MyClass()
{
	throw 0;
}

::vl::Ptr<::IMyInterface3> MyClass::MyClass()
{
	throw 0;
}

