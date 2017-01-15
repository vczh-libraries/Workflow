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
		auto a = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(1));
		auto b = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(2));
		auto c = ::vl::Ptr<::test::ObservableValue>();
		return ((((((((((((((((((((::vl::__vwsn::ToString((! static_cast<bool>(a))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<bool>(a))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((a.Obj() == b.Obj()))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((! static_cast<bool>(b)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<bool>(b))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((a.Obj() != b.Obj()))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((a.Obj() == c.Obj()))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((b.Obj() == c.Obj()))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((a.Obj() == a.Obj()))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((b.Obj() == b.Obj()))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString((c.Obj() == c.Obj())));
	}

	OpCompareReference& OpCompareReference::Instance()
	{
		return Getvl_workflow_global_OpCompareReference().instance;
	}
}

