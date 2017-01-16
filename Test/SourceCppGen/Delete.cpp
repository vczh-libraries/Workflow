/***********************************************************************
Generated from ../Resources/Codegen/Delete.txt
***********************************************************************/

#include "Delete.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::Delete::
#define GLOBAL_NAME ::vl_workflow_global::Delete::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::Delete::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Delete)
	vl_workflow_global::Delete instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Delete)

namespace vl_workflow_global
{
	bool Delete::DoDelete(::test::ObservableValue* value)
	{
		try
		{
			{
				::vl::__vwsn::This(value)->Dispose(true);
				return true;
			}
		}
		catch(const ::vl::Exception& __vwsne_0)
		{
			auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
			{
				return false;
			}
		}
		catch(const ::vl::Error& __vwsne_0)
		{
			auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
			{
				return false;
			}
		}
	}

	::vl::WString Delete::main()
	{
		auto raw1 = ::test::ObservableValue::CreatePtr(0, ::vl::WString(L"", false));
		auto raw2 = ::test::ObservableValue::CreatePtr(0, ::vl::WString(L"", false));
		auto shared = ::vl::__vwsn::Ensure(::vl::Ptr<::test::ObservableValue>(raw1));
		auto a = (raw1 == nullptr);
		auto b = (raw2 == nullptr);
		auto c = (! static_cast<bool>(shared));
		auto d = GLOBAL_NAME DoDelete(raw1);
		auto e = GLOBAL_NAME DoDelete(raw2);
		return ((((((((::vl::__vwsn::ToString(a) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(b)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(c)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(d)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(e));
	}

	Delete& Delete::Instance()
	{
		return Getvl_workflow_global_Delete().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
