/***********************************************************************
Generated from ../Resources/Codegen/OrderedLambda.txt
***********************************************************************/

#include "OrderedLambda.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OrderedLambda)
	vl_workflow_global::OrderedLambda instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OrderedLambda)

namespace vl_workflow_global
{

	struct __vwsno1_OrderedLambda_Adder_
	{
		::vl::vint32_t x;

		__vwsno1_OrderedLambda_Adder_(::vl::vint32_t __vwsnctor_x);

		::vl::vint32_t operator()(::vl::vint32_t __vwsno_1) const;
	};

	struct __vwsno2_OrderedLambda_Adder_
	{
		::vl::vint32_t __vwsno_1;

		__vwsno2_OrderedLambda_Adder_(::vl::vint32_t __vwsnctor___vwsno_1);

		::vl::vint32_t operator()(::vl::vint32_t __vwsno_2) const;
	};
	::vl::vint32_t OrderedLambda::Apply(const ::vl::Func<::vl::vint32_t(::vl::vint32_t)>& f, ::vl::vint32_t arg)
	{
		return f(arg);
	}

	::vl::Func<::vl::vint32_t(::vl::vint32_t)> OrderedLambda::Adder(::vl::vint32_t x)
	{
		return LAMBDA(::vl_workflow_global::__vwsno1_OrderedLambda_Adder_(x));
	}

	::vl::WString OrderedLambda::main()
	{
		::vl::Func<::vl::vint32_t(::vl::vint32_t)> f = ::vl_workflow_global::OrderedLambda::Instance().Adder(1);
		::vl::Func<::vl::vint32_t(::vl::vint32_t)> g = ::vl_workflow_global::OrderedLambda::Instance().Adder(2);
		return (((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(f(1), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(f(2), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(g(1), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(g(2), __vwsn_temp__); return __vwsn_temp__; }());
	}

	OrderedLambda& OrderedLambda::Instance()
	{
		return Getvl_workflow_global_OrderedLambda().instance;
	}

	__vwsno1_OrderedLambda_Adder_::__vwsno1_OrderedLambda_Adder_(::vl::vint32_t __vwsnctor_x)
		:x(__vwsnctor_x)
	{
	}

	::vl::vint32_t __vwsno1_OrderedLambda_Adder_::operator()(::vl::vint32_t __vwsno_1) const
	{
		return ((__vwsno_1 * 0) + ::vl_workflow_global::OrderedLambda::Instance().Apply(LAMBDA(::vl_workflow_global::__vwsno2_OrderedLambda_Adder_(__vwsno_1)), x));
	}

	__vwsno2_OrderedLambda_Adder_::__vwsno2_OrderedLambda_Adder_(::vl::vint32_t __vwsnctor___vwsno_1)
		:__vwsno_1(__vwsnctor___vwsno_1)
	{
	}

	::vl::vint32_t __vwsno2_OrderedLambda_Adder_::operator()(::vl::vint32_t __vwsno_2) const
	{
		return (__vwsno_1 + __vwsno_2);
	}
}

