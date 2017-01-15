/***********************************************************************
Generated from ../Resources/Codegen/ElementInSet.txt
***********************************************************************/

#include "ElementInSet.h"

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
		::vl::collections::LazyList<::vl::vint32_t> xs = ::vl::reflection::description::GetLazyList<::vl::vint32_t>([&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add(::vl::__vwsn::Box(1)); __vwsn_temp__->Add(::vl::__vwsn::Box(2)); __vwsn_temp__->Add(::vl::__vwsn::Box(3)); __vwsn_temp__->Add(::vl::__vwsn::Box(4)); __vwsn_temp__->Add(::vl::__vwsn::Box(5)); return __vwsn_temp__; }());
		::vl::Ptr<::vl::reflection::description::IValueList> ys = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add(::vl::__vwsn::Box(1)); __vwsn_temp__->Add(::vl::__vwsn::Box(2)); __vwsn_temp__->Add(::vl::__vwsn::Box(3)); __vwsn_temp__->Add(::vl::__vwsn::Box(4)); __vwsn_temp__->Add(::vl::__vwsn::Box(5)); return __vwsn_temp__; }();
		::vl::collections::LazyList<::vl::vint32_t> zs = ::vl::__vwsn::Range(1 + 1, 5);
		return ((((((((((((((((((::vl::__vwsn::ToString(::vl::__vwsn::InSet(3, xs)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(0, xs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(3, ys))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(0, ys))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(3, zs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(1, zs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::InSet(5, zs))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString([&](auto __vwsn_1){ return (1.0 <  __vwsn_1 && __vwsn_1 < 5); }(3))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString([&](auto __vwsn_1){ return (1 <=  __vwsn_1 && __vwsn_1 <= 5); }(1))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString([&](auto __vwsn_1){ return (1 <=  __vwsn_1 && __vwsn_1 <= 5); }(0)));
	}

	ElementInSet& ElementInSet::Instance()
	{
		return Getvl_workflow_global_ElementInSet().instance;
	}
}

