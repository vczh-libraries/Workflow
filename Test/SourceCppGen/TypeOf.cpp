/***********************************************************************
Generated from ../Resources/Codegen/TypeOf.txt
***********************************************************************/

#include "TypeOf.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TypeOf)
	vl_workflow_global::TypeOf instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TypeOf)

namespace vl_workflow_global
{
	::vl::WString TypeOf::main()
	{
		::vl::reflection::description::ITypeDescriptor* a = /* NOT SUPPORTS: typeof() */ __vwsn_not_exists__;
		::vl::reflection::description::ITypeDescriptor* b = /* NOT SUPPORTS: typeof() */ __vwsn_not_exists__;
		::vl::reflection::description::ITypeDescriptor* c = /* NOT SUPPORTS: type() */ __vwsn_not_exists__;
		::vl::reflection::description::ITypeDescriptor* d = /* NOT SUPPORTS: type() */ __vwsn_not_exists__;
		return (((((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a == c), __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((a == d), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((b == c), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize((b == d), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(a)->GetTypeName()) + ::vl::WString(L", ", false)) + ::vl::__vwsn::This(b)->GetTypeName());
	}

	TypeOf& TypeOf::Instance()
	{
		return Getvl_workflow_global_TypeOf().instance;
	}
}

