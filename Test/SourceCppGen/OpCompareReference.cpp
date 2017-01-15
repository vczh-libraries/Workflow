/***********************************************************************
Generated from ../Resources/Codegen/OpCompareReference.txt
***********************************************************************/

#include "OpCompareReference.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompareReference)
	vl_workflow_global::OpCompareReference instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpCompareReference)

namespace vl_workflow_global
{
	::vl::WString OpCompareReference::main()
	{
				::vl::Ptr<::test::ObservableValue> a = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(1));
				::vl::Ptr<::test::ObservableValue> b = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(2));
				::vl::Ptr<::test::ObservableValue> c = nullptr;
				return ((((((((((((((((((((static_cast<bool>((! static_cast<bool>(a))) + ::vl::WString(L", ", false)) + static_cast<bool>(static_cast<bool>(a))) + ::vl::WString(L", ", false)) + static_cast<bool>((a.Obj() == b.Obj()))) + ::vl::WString(L", ", false)) + static_cast<bool>((! static_cast<bool>(b)))) + ::vl::WString(L", ", false)) + static_cast<bool>(static_cast<bool>(b))) + ::vl::WString(L", ", false)) + static_cast<bool>((a.Obj() != b.Obj()))) + ::vl::WString(L", ", false)) + static_cast<bool>((a.Obj() == c.Obj()))) + ::vl::WString(L", ", false)) + static_cast<bool>((b.Obj() == c.Obj()))) + ::vl::WString(L", ", false)) + static_cast<bool>((a.Obj() == a.Obj()))) + ::vl::WString(L", ", false)) + static_cast<bool>((b.Obj() == b.Obj()))) + ::vl::WString(L", ", false)) + static_cast<bool>((c.Obj() == c.Obj())));
	}

	OpCompareReference& OpCompareReference::Instance()
	{
		return Getvl_workflow_global_OpCompareReference().instance;
	}
}

