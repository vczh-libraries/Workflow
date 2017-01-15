/***********************************************************************
Generated from ../Resources/Codegen/MethodClosure.txt
***********************************************************************/

#include "MethodClosure.h"

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
				::vl::Ptr<::test::ObservableValue> x = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(10));
				::vl::Func<::vl::vint32_t()> get = ::vl::Func<::vl::vint32_t()>(::vl::__vwsn::This(x), &::test::ObservableValue::GetValue);
				::vl::Func<void(::vl::vint32_t)> set = ::vl::Func<void(::vl::vint32_t)>(::vl::__vwsn::This(x), &::test::ObservableValue::SetValue);
				::vl::vint32_t s1 = get();
				set(20);
				::vl::vint32_t s2 = get();
				return ((static_cast<::vl::vint32_t>(s1) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(s2));
	}

	MethodClosure& MethodClosure::Instance()
	{
		return Getvl_workflow_global_MethodClosure().instance;
	}
}

