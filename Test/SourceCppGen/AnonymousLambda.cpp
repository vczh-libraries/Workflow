/***********************************************************************
Generated from ../Resources/Codegen/AnonymousLambda.txt
***********************************************************************/

#include "AnonymousLambda.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_AnonymousLambda)
	vl_workflow_global::AnonymousLambda instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_AnonymousLambda)

namespace vl_workflow_global
{

	struct __vwsnf1_AnonymousLambda_Adder_
	{
		::vl::vint32_t x;

		__vwsnf1_AnonymousLambda_Adder_(::vl::vint32_t __vwsnctor_x);

		::vl::Func<::vl::vint32_t()> operator()(::vl::vint32_t y) const;
	};

	struct __vwsnf2_AnonymousLambda_Adder__
	{
		::vl::vint32_t x;
		::vl::vint32_t y;

		__vwsnf2_AnonymousLambda_Adder__(::vl::vint32_t __vwsnctor_x, ::vl::vint32_t __vwsnctor_y);

		::vl::vint32_t operator()() const;
	};
	::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> AnonymousLambda::Adder(::vl::vint32_t x)
	{
		return LAMBDA(::vl_workflow_global::__vwsnf1_AnonymousLambda_Adder_(x));
	}

	::vl::WString AnonymousLambda::main()
	{
		::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> f = ::vl_workflow_global::AnonymousLambda::Instance().Adder(1);
		::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> g = ::vl_workflow_global::AnonymousLambda::Instance().Adder(2);
		return (((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(f(1)(), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(f(2)(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(g(1)(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(g(2)(), __vwsn_temp__); return __vwsn_temp__; }());
	}

	AnonymousLambda& AnonymousLambda::Instance()
	{
		return Getvl_workflow_global_AnonymousLambda().instance;
	}

	__vwsnf1_AnonymousLambda_Adder_::__vwsnf1_AnonymousLambda_Adder_(::vl::vint32_t __vwsnctor_x)
		:x(__vwsnctor_x)
	{
	}

	::vl::Func<::vl::vint32_t()> __vwsnf1_AnonymousLambda_Adder_::operator()(::vl::vint32_t y) const
	{
		return LAMBDA(::vl_workflow_global::__vwsnf2_AnonymousLambda_Adder__(x, y));
	}

	__vwsnf2_AnonymousLambda_Adder__::__vwsnf2_AnonymousLambda_Adder__(::vl::vint32_t __vwsnctor_x, ::vl::vint32_t __vwsnctor_y)
		:x(__vwsnctor_x)
		, y(__vwsnctor_y)
	{
	}

	::vl::vint32_t __vwsnf2_AnonymousLambda_Adder__::operator()() const
	{
		return (x + y);
	}
}

