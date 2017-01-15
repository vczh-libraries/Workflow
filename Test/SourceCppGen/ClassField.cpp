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
		(::vl::__vwsn::This(b)->x);
		(::vl::__vwsn::This(b)->y);
		return (((((((((((::vl::WString(L"", false) + static_cast<::vl::vint32_t>(::vl::__vwsn::This(a)->x)) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl::__vwsn::This(a)->y)) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl::__vwsn::This(b)->x)) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl::__vwsn::This(b)->y)) + ::vl::WString(L", ", false)) + static_cast<bool>((a.Obj() == a.Obj()))) + ::vl::WString(L", ", false)) + static_cast<bool>((a.Obj() == b.Obj())));
	}

	ClassField& ClassField::Instance()
	{
		return Getvl_workflow_global_ClassField().instance;
	}
}

