/***********************************************************************
Generated from ../Resources/Codegen/ClassField.txt
***********************************************************************/

#include "ClassField.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassField)
	vl_workflow_global::ClassField instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassField)

namespace vl_workflow_global
{
	::vl::WString ClassField::main()
	{
		::vl::WString s = ::vl::WString(L"", false);
		::vl::Ptr<::test::PointClass> a = ::vl::Ptr<::test::PointClass>(new ::test::PointClass());
		::vl::Ptr<::test::PointClass> b = ::vl::Ptr<::test::PointClass>(new ::test::PointClass());
		(::vl::__vwsn::This(b.Obj())->x = 1);
		(::vl::__vwsn::This(b.Obj())->y = 2);
		return (((((((((((::vl::WString(L"", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(a.Obj())->x, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(a.Obj())->y, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(b.Obj())->x, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(b.Obj())->y, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a.Obj() == a.Obj()), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a.Obj() == b.Obj()), __vwsn_temp__); return __vwsn_temp__; }());
	}

	ClassField& ClassField::Instance()
	{
		return Getvl_workflow_global_ClassField().instance;
	}
}

