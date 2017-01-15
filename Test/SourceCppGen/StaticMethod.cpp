/***********************************************************************
Generated from ../Resources/Codegen/StaticMethod.txt
***********************************************************************/

#include "StaticMethod.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::StaticMethod::
#define GLOBAL_NAME ::vl_workflow_global::StaticMethod::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::StaticMethod::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)
	vl_workflow_global::StaticMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_StaticMethod)

namespace vl_workflow_global
{
	::vl::Ptr<::test::ObservableValue> StaticMethod::Create2(::vl::vint32_t value, const ::vl::WString& name)
	{
		return ::test::ObservableValue::Create(value, name);
	}

	::vl::WString StaticMethod::main()
	{
		auto create1 = ::vl::Func<::vl::Ptr<::test::ObservableValue>(::vl::vint32_t, const ::vl::WString&)>(&::test::ObservableValue::Create);
		auto create2 = ::vl::Func<::vl::Ptr<::test::ObservableValue>(::vl::vint32_t, const ::vl::WString&)>(GLOBAL_OBJ, &GLOBAL_SYMBOL Create2);
		auto o1 = create1(1, ::vl::WString(L"one", false));
		auto o2 = GLOBAL_NAME Create2(2, ::vl::WString(L"two", false));
		return ((((((::vl::__vwsn::ToString(::vl::__vwsn::This(o1.Obj())->GetValue()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(o1.Obj())->GetDisplayName()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::This(o2.Obj())->GetValue())) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(o2.Obj())->GetDisplayName());
	}

	StaticMethod& StaticMethod::Instance()
	{
		return Getvl_workflow_global_StaticMethod().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
