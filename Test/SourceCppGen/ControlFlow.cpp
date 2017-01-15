/***********************************************************************
Generated from ../Resources/Codegen/ControlFlow.txt
***********************************************************************/

#include "ControlFlow.h"

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
		::vl::vint32_t result = 0;
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, [&](::vl::vint32_t __vwsn_1, ::vl::vint32_t __vwsn_2){ return ::vl::collections::Range<::vl::vint32_t>(__vwsn_1, __vwsn_2 - __vwsn_1); }(start, end + 1));
		{
			(result = (result + x));
		}
		return result;
	}

	::vl::vint32_t ControlFlow::Sum2(::vl::vint32_t start, ::vl::vint32_t end)
	{
		::vl::vint32_t result = 0;
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, [&](::vl::vint32_t __vwsn_1, ::vl::vint32_t __vwsn_2){ return ::vl::collections::Range<::vl::vint32_t>(__vwsn_1, __vwsn_2 - __vwsn_1); }(start, end + 1).Reverse());
		{
			(result = (result + x));
		}
		return result;
	}

	::vl::vint32_t ControlFlow::Sum3(::vl::vint32_t start, ::vl::vint32_t end)
	{
		::vl::vint32_t result = 0;
		::vl::vint32_t current = start;
		while ((current <= end))
		{
			(result = (result + current));
			(current = (current + 1));
		}
		return result;
	}

	::vl::vint32_t ControlFlow::Sum4(::vl::vint32_t start, ::vl::vint32_t end)
	{
		::vl::vint32_t result = 0;
		::vl::vint32_t current = start;
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
		::vl::vint32_t result = 0;
		::vl::vint32_t current = start;
		bool needToExit = false;
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
		return ((((((((static_cast<::vl::vint32_t>(::vl_workflow_global::ControlFlow::Instance().Sum1(1, 10)) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::ControlFlow::Instance().Sum2(1, 10))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::ControlFlow::Instance().Sum3(1, 10))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::ControlFlow::Instance().Sum4(1, 10))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>(::vl_workflow_global::ControlFlow::Instance().Sum5(1, 10)));
	}

	ControlFlow& ControlFlow::Instance()
	{
		return Getvl_workflow_global_ControlFlow().instance;
	}
}

