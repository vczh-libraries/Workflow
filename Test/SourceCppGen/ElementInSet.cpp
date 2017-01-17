/***********************************************************************
Generated from ../Resources/Codegen/ElementInSet.txt
***********************************************************************/

#include "ElementInSet.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::ElementInSet::
#define GLOBAL_NAME ::vl_workflow_global::ElementInSet::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::ElementInSet::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ElementInSet)
	vl_workflow_global::ElementInSet instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ElementInSet)

namespace vl_workflow_global
{
	::vl::WString ElementInSet::main()
	{
		auto xs = ::vl::reflection::description::GetLazyList<::vl::vint32_t>((::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list);
		auto ys = (::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list;
		auto zs = ::vl::__vwsn::Range(1 + 1, 5);
		return ((((((((((((((((((::vl::__vwsn::ToString(::vl::__vwsn::InSet(3, xs)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(0, xs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(3, ys))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(0, ys))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(3, zs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(1, zs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(5, zs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString([&](auto __vwsn_1){ return (1.0 <  __vwsn_1 && __vwsn_1 < 5); }(3))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString([&](auto __vwsn_1){ return (1 <=  __vwsn_1 && __vwsn_1 <= 5); }(1))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString([&](auto __vwsn_1){ return (1 <=  __vwsn_1 && __vwsn_1 <= 5); }(0)));
	}

	ElementInSet& ElementInSet::Instance()
	{
		return Getvl_workflow_global_ElementInSet().instance;
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
