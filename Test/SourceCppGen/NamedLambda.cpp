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

	struct __vwsnf1_NamedLambda_main_
	{

		__vwsnf1_NamedLambda_main_();

		::vl::vint32_t operator()(::vl::vint32_t n) const;
	};
	::vl::WString NamedLambda::main()
	{
		::vl::Func<::vl::vint32_t(::vl::vint32_t)> fib = LAMBDA(::vl_workflow_global::__vwsnf1_NamedLambda_main_());
		return (((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(fib(1), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(fib(2), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(fib(3), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(fib(4), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(fib(5), __vwsn_temp__); return __vwsn_temp__; }());
	}

	NamedLambda& NamedLambda::Instance()
	{
		return Getvl_workflow_global_NamedLambda().instance;
	}

	__vwsnf1_NamedLambda_main_::__vwsnf1_NamedLambda_main_()
	{
	}

	::vl::vint32_t __vwsnf1_NamedLambda_main_::operator()(::vl::vint32_t n) const
	{
		::vl::Func<::vl::vint32_t(::vl::vint32_t)> fib2 = LAMBDA(::vl_workflow_global::__vwsnf1_NamedLambda_main_());
		return [&](){ if ((n <= 2)) return 1; else return ((*this)((n - 1)) + fib2((n - 2))); }();
	}
}

