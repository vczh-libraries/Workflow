/***********************************************************************
Generated from ../Resources/Codegen/WorkflowHints.txt
***********************************************************************/

#include "WorkflowHints.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::WorkflowHints::
#define GLOBAL_NAME ::vl_workflow_global::WorkflowHints::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::WorkflowHints::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_WorkflowHints)
	vl_workflow_global::WorkflowHints instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_WorkflowHints)

namespace vl_workflow_global
{
	void WorkflowHints::Ensure(bool condition)
	{
		if ((! condition))
		{
			throw ::vl::Exception(::vl::WString(L"Wrong!", false));
		}
	}

	void WorkflowHints::ExtraTests()
	{
		GLOBAL_NAME Ensure((::vl::__vwsn::ToString(123) == ::vl::WString(L"123", false)));
		GLOBAL_NAME Ensure((::vl::__vwsn::Parse<::vl::vint32_t>(::vl::WString(L"123", false)) == 123));
		auto sBase = ::vl::Ptr<::Base>(new ::Base());
		auto rBase = new ::Base(0);
		auto sDerived = ::vl::Ptr<::Derived>(new ::Derived());
		auto rDerived = new ::Derived(0);
		auto srBase = ::vl::__vwsn::Ensure(::vl::Ptr<::Base>(rBase));
		auto srDerived = ::vl::__vwsn::Ensure(::vl::Ptr<::Derived>(rDerived));
		GLOBAL_NAME Ensure((::vl::__vwsn::Ensure(static_cast<::Base*>(sDerived.Obj())) != nullptr));
		GLOBAL_NAME Ensure(static_cast<bool>(::vl::__vwsn::Ensure(::vl::Ptr<::Base>(sDerived))));
		GLOBAL_NAME Ensure((::vl::__vwsn::Ensure(static_cast<::Base*>(rDerived) != nullptr));
		GLOBAL_NAME Ensure(static_cast<bool>(::vl::__vwsn::Ensure(::vl::Ptr<::Base>(rDerived))));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(sBase.Obj()) == nullptr));
		GLOBAL_NAME Ensure((! static_cast<bool>(::vl::__vwsn::SharedPtrCast<::Derived>(sBase.Obj()))));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(rBase) == nullptr));
		GLOBAL_NAME Ensure((! static_cast<bool>(::vl::__vwsn::SharedPtrCast<::Derived>(rBase))));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(sBase.Obj()) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(sBase.Obj()) != nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(sBase.Obj()) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(sBase.Obj()) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(rBase) != nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(rBase) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(rBase) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(rBase) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(sDerived.Obj()) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(sDerived.Obj()) != nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(sDerived.Obj()) == nullptr));
		GLOBAL_NAME Ensure((! (::vl::__vwsn::RawPtrCast<::Derived>(sDerived.Obj()) == nullptr)));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(rDerived) != nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Base>(rDerived) == nullptr));
		GLOBAL_NAME Ensure((! (::vl::__vwsn::RawPtrCast<::Derived>(rDerived) == nullptr)));
		GLOBAL_NAME Ensure((::vl::__vwsn::RawPtrCast<::Derived>(rDerived) == nullptr));
		auto iNull = ::vl::Nullable<::vl::vint32_t>();
		auto iValue = ::vl::Nullable<::vl::vint32_t>(static_cast<::vl::vint32_t>(1));
		auto sNull = ::vl::Nullable<::vl::WString>();
		auto sValue = ::vl::Nullable<::vl::WString>(::vl::__vwsn::Parse<::vl::WString>(::vl::WString(L"2", false)));
		GLOBAL_NAME Ensure((! static_cast<bool>(iNull)));
		GLOBAL_NAME Ensure(static_cast<bool>(iValue));
		GLOBAL_NAME Ensure((! static_cast<bool>(sNull)));
		GLOBAL_NAME Ensure(static_cast<bool>(sValue));
		GLOBAL_NAME Ensure((! static_cast<bool>(::vl::__vwsn::NullableCast<::vl::WString>(iNull))));
		GLOBAL_NAME Ensure((::vl::__vwsn::NullableCast<::vl::WString>(iValue) == ::vl::WString(L"1", false)));
		GLOBAL_NAME Ensure((::vl::__vwsn::ToString(iValue.Value()) == ::vl::WString(L"1", false)));
		GLOBAL_NAME Ensure((! static_cast<bool>(::vl::__vwsn::NullableCast<::vl::vint32_t>(sNull))));
		GLOBAL_NAME Ensure((::vl::__vwsn::NullableCast<::vl::vint32_t>(sValue) == 2));
		GLOBAL_NAME Ensure((::vl::__vwsn::Parse<::vl::vint32_t>(sValue.Value()) == 2));
		GLOBAL_NAME Ensure((! static_cast<bool>(iNull)));
		GLOBAL_NAME Ensure(static_cast<bool>(iValue));
		GLOBAL_NAME Ensure((! static_cast<bool>(sNull)));
		GLOBAL_NAME Ensure(static_cast<bool>(sValue));
		auto x = static_cast<::vl::reflection::description::Value>(nullptr);
		auto y = ::vl::__vwsn::Box(2);
		GLOBAL_NAME Ensure(x.IsNull());
		GLOBAL_NAME Ensure((! y.IsNull()));
		GLOBAL_NAME Ensure((dynamic_cast<::vl::reflection::description::IValueType::TypedBox<::vl::vint32_t>*>(x.GetBoxedValue().Obj()) == nullptr));
		GLOBAL_NAME Ensure((dynamic_cast<::vl::reflection::description::IValueType::TypedBox<::vl::vint32_t>*>(y.GetBoxedValue().Obj()) != nullptr));
		auto z = ::vl::__vwsn::Box(rBase);
		GLOBAL_NAME Ensure((! z.IsNull()));
		GLOBAL_NAME Ensure((::vl::__vwsn__RawPtrCast<::Base>(z.GetRawPtr()) != nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn__RawPtrCast<::Base>(z.GetRawPtr()) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn__RawPtrCast<::Derived>(z.GetRawPtr()) == nullptr));
		GLOBAL_NAME Ensure((::vl::__vwsn__RawPtrCast<::Derived>(z.GetRawPtr()) == nullptr));
	}

	::vl::WString WorkflowHints::main()
	{
		GLOBAL_NAME ExtraTests();
		auto hinters = ::vl::Ptr<::test::Hinters>(::test::CreateHinter());
		{
			auto hinters2 = ::test::CreateHinter(0);
			::vl::__vwsn::This(hinters2)->Dispose(true);
		}
		auto a = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box((::vl::__vwsn::CreateList().Add(1).Add(2).Add(3)).list); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); auto __vwsn_temp_1 = 4; return ::vl::__vwsn::This(hinters.Obj())->GetList(__vwsn_temp_0, __vwsn_temp_1); }()[0];
		auto b = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(10).Add(20).Add(30)).list))); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::vl::__vwsn::This(hinters.Obj())->GetReadonlyList(__vwsn_temp_0); }()[0];
		auto c = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This([&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(100).Add(200).Add(300)).list))); ::vl::collections::Array<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return [&](){ decltype(auto) __vwsn_temp__ = ::vl::__vwsn::This(hinters.Obj())->GetReadonlyArray(__vwsn_temp_0); return ::vl::__vwsn::Unbox<::vl::Ptr<::vl::reflection::description::IValueReadonlyList>>(::vl::reflection::description::BoxParameter<decltype(__vwsn_temp__)>(__vwsn_temp__)); }(); }().Obj())->Get(0));
		auto d = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This([&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyList>((::vl::__vwsn::CreateList().Add(1000).Add(2000).Add(3000)).list))); ::vl::collections::SortedList<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return [&](){ decltype(auto) __vwsn_temp__ = ::vl::__vwsn::This(hinters.Obj())->GetReadonlySL(__vwsn_temp_0); return ::vl::__vwsn::Unbox<::vl::Ptr<::vl::reflection::description::IValueReadonlyList>>(::vl::reflection::description::BoxParameter<decltype(__vwsn_temp__)>(__vwsn_temp__)); }(); }().Obj())->Get(0));
		auto e = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box((::vl::__vwsn::CreateDictionary().Add(1, 2).Add(3, 4)).dictionary); ::vl::collections::Dictionary<::vl::vint32_t, ::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::test::GetDictionaryFromHinter(::vl::__vwsn::This(hinters.Obj()), __vwsn_temp_0); }()[1];
		auto f = [&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueReadonlyDictionary>((::vl::__vwsn::CreateDictionary().Add(10, 20).Add(30, 40)).dictionary))); ::vl::collections::Dictionary<::vl::vint32_t, ::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::test::GetReadonlyDictionaryFromHinter(::vl::__vwsn::This(hinters.Obj()), __vwsn_temp_0); }()[10];
		[&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box((::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::vl::__vwsn::This(hinters.Obj())->SetStorage(__vwsn_temp_0); }();
		auto g1 = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This([&](){ decltype(auto) __vwsn_temp__ = ::vl::__vwsn::This(hinters.Obj())->GetStorage(); return ::vl::__vwsn::Unbox<::vl::Ptr<::vl::reflection::description::IValueList>>(::vl::reflection::description::BoxParameter<decltype(__vwsn_temp__)>(__vwsn_temp__)); }().Obj())->Get(0));
		auto xs = (::vl::__vwsn::CreateList().Add(1).Add(2).Add(3).Add(4).Add(5)).list;
		[&]()->decltype(auto){ auto __vwsn_temp_x0 = ::vl::__vwsn::Box(xs); ::vl::collections::List<::vl::vint32_t> __vwsn_temp_0; ::vl::reflection::description::UnboxParameter(__vwsn_temp_x0, __vwsn_temp_0); return ::vl::__vwsn::This(hinters.Obj())->SetStorage(__vwsn_temp_0); }();
		::vl::__vwsn::This(hinters.Obj())->GetStorage().Set(4, 100);
		auto g2 = ::vl::__vwsn::This(hinters.Obj())->GetStorage()[4];
		return ((((((((((((((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(a)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(b)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(c)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(d)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(e)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(f)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(g1)) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(g2)) + ::vl::WString(L"]", false));
	}

	WorkflowHints& WorkflowHints::Instance()
	{
		return Getvl_workflow_global_WorkflowHints().instance;
	}
}

/***********************************************************************
Class (::Base)
***********************************************************************/

Base::Base()
{
}

Base::Base(::vl::vint32_t x)
{
}

/***********************************************************************
Class (::Derived)
***********************************************************************/

Derived::Derived()
{
}

Derived::Derived(::vl::vint32_t x)
{
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
