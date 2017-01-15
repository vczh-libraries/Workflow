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
		::vl::collections::LazyList<::vl::vint32_t> xs = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 2; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 3; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 4; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 5; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }().Cast<::vl::reflection::description::IValueList>();
		::vl::Ptr<::vl::reflection::description::IValueList> ys = [&](){ auto __vwsn_temp__ = ::vl::reflection::description::IValueList::Create(); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 1; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 2; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 3; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 4; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); __vwsn_temp__->Add([&](){ ::vl::vint32_t __vwsn_temp__ = 5; return ::vl::reflection::description::BoxParameter<::vl::vint32_t>(__vwsn_temp__); }()); return __vwsn_temp__; }();
		::vl::collections::LazyList<::vl::vint32_t> zs = [&](::vl::vint32_t __vwsn_1, ::vl::vint32_t __vwsn_2){ return ::vl::collections::Range<::vl::vint32_t>(__vwsn_1, __vwsn_2 - __vwsn_1); }(1 + 1, 5);
		return ((((((((((((((((((static_cast<bool>([&](::vl::vint32_t __vwsn_1){ return xs.Any([&](::vl::vint32_t __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(3)) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1){ return xs.Any([&](::vl::vint32_t __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(0))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1){ return ::vl::reflection::description::GetLazyList<::vl::vint32_t(ys).Any([&](::vl::vint32_t __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(3))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1){ return ::vl::reflection::description::GetLazyList<::vl::vint32_t(ys).Any([&](::vl::vint32_t __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(0))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1){ return zs.Any([&](::vl::vint32_t __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(3))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1){ return zs.Any([&](::vl::vint32_t __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(1))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1){ return zs.Any([&](::vl::vint32_t __vwsn_2){ return __vwsn_1 == __vwsn_2; }); }(5))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1{ return 1.0 <  __vwsn_1 && __vwsn_1 < 5; }(3))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1{ return 1 <=  __vwsn_1 && __vwsn_1 <= 5; }(1))) + ::vl::WString(L", ", false)) + static_cast<bool>([&](::vl::vint32_t __vwsn_1{ return 1 <=  __vwsn_1 && __vwsn_1 <= 5; }(0)));
	}

	ElementInSet& ElementInSet::Instance()
	{
		return Getvl_workflow_global_ElementInSet().instance;
	}
}

