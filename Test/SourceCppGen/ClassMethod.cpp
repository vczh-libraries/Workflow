/***********************************************************************
Generated from ../Resources/Codegen/ClassMethod.txt
***********************************************************************/

#include "ClassMethod.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassMethod)
	vl_workflow_global::ClassMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassMethod)

namespace vl_workflow_global
{

	struct __vwsnf1_ClassMethod_main_
	{
		::vl::Ptr<::Methods> a;

		__vwsnf1_ClassMethod_main_(::vl::Ptr<::Methods> __vwsnctor_a);

		void operator()() const;
	};
	::vl::WString ClassMethod::main()
	{
		{
			::vl::Ptr<::Methods> a = ::vl::Ptr<::Methods>(new ::Methods());
			(::vl_workflow_global::ClassMethod::Instance().s = (::vl_workflow_global::ClassMethod::Instance().s + ((::vl::WString(L"[", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(a)->x, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"]", false))));
		}
		{
			::vl::Ptr<::Methods> a = ::vl::Ptr<::Methods>(new ::Methods(100));
			(::vl_workflow_global::ClassMethod::Instance().s = (::vl_workflow_global::ClassMethod::Instance().s + ((::vl::WString(L"[", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(a)->x, __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"]", false))));
		}
		{
			::vl::Ptr<::Methods> a = ::vl::Ptr<::Methods>(new ::Methods(1));
			::vl::Ptr<::vl::reflection::description::IEventHandler> handler = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(a)->XChanged, LAMBDA(::vl_workflow_global::__vwsnf1_ClassMethod_main_(a)));
			::vl::__vwsn::This(a)->SetX((::vl::__vwsn::This(a)->GetX() + 1));
			::vl::__vwsn::This(a)->SetX((::vl::__vwsn::This(a)->GetX() - 1));
			::vl::__vwsn::This(a)->SetX(::vl::__vwsn::This(a)->GetX());
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(a)->XChanged, handler);
			::vl::__vwsn::This(a)->SetX(100);
			(::vl_workflow_global::ClassMethod::Instance().s = (::vl_workflow_global::ClassMethod::Instance().s + ((::vl::WString(L"[", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(a)->GetX(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"]", false))));
		}
		return ::vl_workflow_global::ClassMethod::Instance().s;
	}

	ClassMethod& ClassMethod::Instance()
	{
		return Getvl_workflow_global_ClassMethod().instance;
	}

	__vwsnf1_ClassMethod_main_::__vwsnf1_ClassMethod_main_(::vl::Ptr<::Methods> __vwsnctor_a)
		:a(__vwsnctor_a)
	{
	}

	void __vwsnf1_ClassMethod_main_::operator()() const
	{
		(::vl_workflow_global::ClassMethod::Instance().s = (::vl_workflow_global::ClassMethod::Instance().s + ((::vl::WString(L"[", false) + [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl::__vwsn::This(::vl::__vwsn::This(this)->a)->GetX(), __vwsn_temp__); return __vwsn_temp__; }()) + ::vl::WString(L"]", false))));
	}
}

/***********************************************************************
Class (::Methods)
***********************************************************************/

Methods::Methods()
{
}

Methods::Methods(::vl::vint32_t _x)
{
	(::vl::__vwsn::This(this)->x = _x);
	if ((::vl::__vwsn::This(this)->x != ::vl::__vwsn::This(this)->x))
	{
		throw ::vl::Exception(::vl::WString(L"The \"this\" expression is not correctly implemented.", false));
	}
}

::vl::vint32_t Methods::GetX()
{
	if ((::vl::__vwsn::This(this)->x != ::vl::__vwsn::This(this)->x))
	{
		throw ::vl::Exception(::vl::WString(L"The \"this\" expression is not correctly implemented.", false));
	}
	return ::vl::__vwsn::This(this)->x;
}

void Methods::SetX(::vl::vint32_t _x)
{
	if ((::vl::__vwsn::This(this)->x != _x))
	{
		(::vl::__vwsn::This(this)->x = _x);
		::vl::__vwsn::EventInvoke(::vl::__vwsn::This(this)->XChanged)();
	}
}

