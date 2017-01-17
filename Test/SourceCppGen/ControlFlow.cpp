/***********************************************************************
Generated from ../Resources/Codegen/ControlFlow.txt
***********************************************************************/

#include "ControlFlow.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::ControlFlow::
#define GLOBAL_NAME ::vl_workflow_global::ControlFlow::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::ControlFlow::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ControlFlow)
	vl_workflow_global::ControlFlow instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ControlFlow)

namespace vl_workflow_global
{
	::vl::vint32_t ControlFlow::Sum1(::vl::vint32_t start, ::vl::vint32_t end)
	{
		auto result = 0;
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, ::vl::__vwsn::Range(start, end + 1))
		{
			(result = (result + x));
		}
		return result;
	}

	::vl::vint32_t ControlFlow::Sum2(::vl::vint32_t start, ::vl::vint32_t end)
	{
		auto result = 0;
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, ::vl::__vwsn::Range(start, end + 1).Reverse())
		{
			(result = (result + x));
		}
		return result;
	}

	::vl::vint32_t ControlFlow::Sum3(::vl::vint32_t start, ::vl::vint32_t end)
	{
		auto result = 0;
		auto current = start;
		while ((current <= end))
		{
			(result = (result + current));
			(current = (current + 1));
		}
		return result;
	}

	::vl::vint32_t ControlFlow::Sum4(::vl::vint32_t start, ::vl::vint32_t end)
	{
		auto result = 0;
		auto current = start;
		while (true)
		{
			if ((current > end))
			{
				break;
			}
			(result = (result + current));
			(current = (current + 1));
		}
		return result;
	}

	::vl::vint32_t ControlFlow::Sum5(::vl::vint32_t start, ::vl::vint32_t end)
	{
		auto result = 0;
		auto current = start;
		auto needToExit = false;
		while ((! needToExit))
		{
			{
				::vl::vint32_t __vwsne_0 = current;
				if (__vwsne_0 == (end + 1))
				{
					(needToExit = true);
				}
				else 				{
					(result = (result + current));
				}
			}
			(current = (current + 1));
		}
		return result;
	}

	::vl::WString ControlFlow::main()
	{
		return ((((((((::vl::__vwsn::ToString(GLOBAL_NAME Sum1(1, 10)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Sum2(1, 10))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Sum3(1, 10))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Sum4(1, 10))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(GLOBAL_NAME Sum5(1, 10)));
	}

	ControlFlow& ControlFlow::Instance()
	{
		return Getvl_workflow_global_ControlFlow().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
