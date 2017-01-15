/***********************************************************************
Generated from ../Resources/Codegen/NamedLambda.txt
***********************************************************************/

#include "NamedLambda.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NamedLambda)
	vl_workflow_global::NamedLambda instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NamedLambda)

namespace vl_workflow_global
{
	::vl::WString NamedLambda::main()
	{
				::vl::Func<::vl::vint32_t(::vl::vint32_t)> fib = LAMBDA(::vl_workflow_global::__vwsnf1_NamedLambda_main_());
				return ((((((((static_cast<::vl::vint32_t>(fib(1)) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(fib(2))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(fib(3))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(fib(4))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(fib(5)));
	}

	NamedLambda& NamedLambda::Instance()
	{
		return Getvl_workflow_global_NamedLambda().instance;
	}

	struct __vwsnf1_NamedLambda_main_
	{

		__vwsnf1_NamedLambda_main_();

		::vl::vint32_t operator()(::vl::vint32_t n) const;
	};

	__vwsnf1_NamedLambda_main_::__vwsnf1_NamedLambda_main_()
	{
	}

	::vl::vint32_t __vwsnf1_NamedLambda_main_::operator()(::vl::vint32_t n) const
	{
				return [&](){ if ((n <= 2)) return 1; else return (::vl_workflow_global::NamedLambda::Instance().Fib((n - 1)) + ::vl_workflow_global::NamedLambda::Instance().Fib((n - 2))); }();
	}
}

