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
	::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> AnonymousLambda::Adder(::vl::vint32_t x)
	{
		return LAMBDA(::vl_workflow_global::__vwsnf1_AnonymousLambda_Adder_(x));
	}

	::vl::WString AnonymousLambda::main()
	{
		::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> f = ::vl_workflow_global::AnonymousLambda::Instance().Adder(1);
		::vl::Func<::vl::Func<::vl::vint32_t()>(::vl::vint32_t)> g = ::vl_workflow_global::AnonymousLambda::Instance().Adder(2);
		return ((((((static_cast<::vl::vint32_t>(f(1)()) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(f(2)())) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(g(1)())) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(g(2)()));
	}

	AnonymousLambda& AnonymousLambda::Instance()
	{
		return Getvl_workflow_global_AnonymousLambda().instance;
	}

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

