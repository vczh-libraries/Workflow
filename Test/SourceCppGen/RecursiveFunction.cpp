/***********************************************************************
Generated from ../Resources/Codegen/RecursiveFunction.txt
***********************************************************************/

#include "RecursiveFunction.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_RecursiveFunction)
	vl_workflow_global::RecursiveFunction instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_RecursiveFunction)

namespace vl_workflow_global
{
	::vl::vint32_t RecursiveFunction::Fib(::vl::vint32_t n)
	{
		return [&](){ if ((n <= 2)) return 1; else return (::vl_workflow_global::RecursiveFunction::Instance().Fib((n - 1)) + ::vl_workflow_global::RecursiveFunction::Instance().Fib((n - 2))); }();
	}

	::vl::WString RecursiveFunction::main()
	{
		return (((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::RecursiveFunction::Instance().Fib(1), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::RecursiveFunction::Instance().Fib(2), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::RecursiveFunction::Instance().Fib(3), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::RecursiveFunction::Instance().Fib(4), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::RecursiveFunction::Instance().Fib(5), __vwsn_temp__); return __vwsn_temp__; }());
	}

	RecursiveFunction& RecursiveFunction::Instance()
	{
		return Getvl_workflow_global_RecursiveFunction().instance;
	}
}

