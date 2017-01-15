/***********************************************************************
Generated from ../Resources/Codegen/Property.txt
***********************************************************************/

#include "Property.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Property)
	vl_workflow_global::Property instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Property)

namespace vl_workflow_global
{
	::vl::WString Property::main()
	{
				::test::Point x = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = 10; __vwsn_temp__.y = 20; return __vwsn_temp__; }();
				::vl::Ptr<::test::ObservableValue> y = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(10));
				::vl::WString s1 = ((((((((::vl::__vwsn::This(x). + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(x).) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(y)->GetValue()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(y)->GetName()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(y)->GetDisplayName());
				::vl::__vwsn::This(y)->SetValue(20);
				::vl::__vwsn::This(y)->SetName(::vl::WString(L"Workflow", false));
				::vl::WString s2 = ((((::vl::__vwsn::This(y)->GetValue() + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(y)->GetName()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(y)->GetDisplayName());
				return ((s1 + ::vl::WString(L", ", false)) + s2);
	}

	Property& Property::Instance()
	{
		return Getvl_workflow_global_Property().instance;
	}
}

