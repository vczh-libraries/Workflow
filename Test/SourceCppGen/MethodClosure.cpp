/***********************************************************************
Generated from ../Resources/Codegen/MethodClosure.txt
***********************************************************************/

#include "MethodClosure.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::MethodClosure::
#define GLOBAL_NAME ::vl_workflow_global::MethodClosure::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::MethodClosure::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_MethodClosure)
	vl_workflow_global::MethodClosure instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_MethodClosure)

namespace vl_workflow_global
{
	::vl::WString MethodClosure::main()
	{
		auto x = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(10));
		auto get = ::vl::Func<::vl::vint32_t()>(::vl::__vwsn::This(x.Obj()), &::test::ObservableValue::GetValue);
		auto set = ::vl::Func<void(::vl::vint32_t)>(::vl::__vwsn::This(x.Obj()), &::test::ObservableValue::SetValue);
		auto s1 = get();
		set(20);
		auto s2 = get();
		return ((::vl::__vwsn::ToString(s1) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(s2));
	}

	MethodClosure& MethodClosure::Instance()
	{
		return Getvl_workflow_global_MethodClosure().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
