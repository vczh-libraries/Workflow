/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/Property.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "Property.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::Property::
#define GLOBAL_NAME ::vl_workflow_global::Property::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::Property::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Property)
	vl_workflow_global::Property instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Property)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString Property::main()
	{
		auto x = [&](){ ::test::Point __vwsn_temp__; __vwsn_temp__.x = static_cast<::vl::vint>(10); __vwsn_temp__.y = static_cast<::vl::vint>(20); return __vwsn_temp__; }();
		auto y = ::vl::Ptr<::test::ObservableValue>(new ::test::ObservableValue(static_cast<::vl::vint>(10)));
		auto s1 = ((((((((::vl::__vwsn::ToString(x.x) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(x.y)) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::ToString(::vl::__vwsn::This(y.Obj())->GetValue())) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::This(y.Obj())->GetName()) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::This(y.Obj())->GetDisplayName());
		::vl::__vwsn::This(y.Obj())->SetValue(static_cast<::vl::vint>(20));
		::vl::__vwsn::This(y.Obj())->SetName(::vl::WString::Unmanaged(L"Workflow"));
		auto s2 = ((((::vl::__vwsn::ToString(::vl::__vwsn::This(y.Obj())->GetValue()) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::This(y.Obj())->GetName()) + ::vl::WString::Unmanaged(L", ")) + ::vl::__vwsn::This(y.Obj())->GetDisplayName());
		return ((s1 + ::vl::WString::Unmanaged(L", ")) + s2);
	}

	Property& Property::Instance()
	{
		return Getvl_workflow_global_Property().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
