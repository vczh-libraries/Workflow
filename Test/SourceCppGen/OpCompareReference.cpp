/***********************************************************************
Generated from ../Resources/Codegen/OpCompareReference.txt
***********************************************************************/

#include "OpCompareReference.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::OpCompareReference::
#define GLOBAL_NAME ::vl_workflow_global::OpCompareReference::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::OpCompareReference::Instance()

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

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
