/***********************************************************************
Generated from ../Resources/Codegen/ClassCtor.txt
***********************************************************************/

#include "ClassCtor.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassCtor)
	vl_workflow_global::ClassCtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassCtor)

namespace vl_workflow_global
{
	::vl::WString ClassCtor::main()
	{
		{
			auto a = ::vl::Ptr<::Ctor>(new ::Ctor());
			(::vl_workflow_global::ClassCtor::Instance().s = (::vl_workflow_global::ClassCtor::Instance().s + ((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetX())) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetValue())) + ::vl::WString(L"]", false))));
		}
		{
			auto a = ::vl::Ptr<::Ctor>(new ::Ctor(1, true));
			(::vl_workflow_global::ClassCtor::Instance().s = (::vl_workflow_global::ClassCtor::Instance().s + ((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetX())) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetValue())) + ::vl::WString(L"]", false))));
		}
		{
			auto a = ::vl::Ptr<::Ctor>(new ::Ctor(true, 2));
			(::vl_workflow_global::ClassCtor::Instance().s = (::vl_workflow_global::ClassCtor::Instance().s + ((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetX())) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetValue())) + ::vl::WString(L"]", false))));
		}
		{
			auto a = ::vl::Ptr<::Ctor>(new ::Ctor(1, 2));
			(::vl_workflow_global::ClassCtor::Instance().s = (::vl_workflow_global::ClassCtor::Instance().s + ((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetX())) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetValue())) + ::vl::WString(L"]", false))));
		}
		return ::vl_workflow_global::ClassCtor::Instance().s;
	}

	ClassCtor& ClassCtor::Instance()
	{
		return Getvl_workflow_global_ClassCtor().instance;
	}
}

/***********************************************************************
Class (::Base)
***********************************************************************/

Base::Base()
{
}

Base::Base(::vl::vint32_t _x)
{
	(::vl::__vwsn::This(this)->x = _x);
	if ((::vl::__vwsn::This(this)->x != ::vl::__vwsn::This(this)->x))
	{
		throw ::vl::Exception(::vl::WString(L"The \"this\" expression is not correctly implemented.", false));
	}
}

::vl::vint32_t Base::GetX()
{
	if ((::vl::__vwsn::This(this)->x != ::vl::__vwsn::This(this)->x))
	{
		throw ::vl::Exception(::vl::WString(L"The \"this\" expression is not correctly implemented.", false));
	}
	return ::vl::__vwsn::This(this)->x;
}

void Base::SetX(::vl::vint32_t _x)
{
	if ((::vl::__vwsn::This(this)->x != _x))
	{
		(::vl::__vwsn::This(this)->x = _x);
		::vl::__vwsn::EventInvoke(::vl::__vwsn::This(this)->XChanged)();
	}
}

/***********************************************************************
Class (::Ctor)
***********************************************************************/

Ctor::Ctor()
{
}

Ctor::Ctor(::vl::vint32_t y, bool b)
{
}

Ctor::Ctor(bool b, ::vl::vint32_t value)
{
}

Ctor::Ctor(::vl::vint32_t y, ::vl::vint32_t value)
{
}

