/***********************************************************************
Generated from ../Resources/Codegen/Delete.txt
***********************************************************************/

#include "Delete.h"

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
		::test::ObservableValue* raw1 = ::test::ObservableValue::CreatePtr(0, ::vl::WString(L"", false));
		::test::ObservableValue* raw2 = ::test::ObservableValue::CreatePtr(0, ::vl::WString(L"", false));
		::vl::Ptr<::test::ObservableValue> shared = ::vl::Ptr<::vl::Ptr<::test::ObservableValue>>(raw1);
		bool a = (raw1 == nullptr);
		bool b = (raw2 == nullptr);
		bool c = (! static_cast<bool>(shared));
		bool d = ::vl_workflow_global::Delete::Instance().DoDelete(raw1);
		bool e = ::vl_workflow_global::Delete::Instance().DoDelete(raw2);
		return (((((((([&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(a, __vwsn_temp__); return __vwsn_temp__; }() + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(b, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(c, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(d, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L", ", false)) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<bool>::Serialize(e, __vwsn_temp__); return __vwsn_temp__; }());
	}

	Delete& Delete::Instance()
	{
		return Getvl_workflow_global_Delete().instance;
	}
}

