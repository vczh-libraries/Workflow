/***********************************************************************
Generated from ../Resources/Codegen/ClassMethod.txt
***********************************************************************/

#include "ClassMethod.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::ClassMethod::
#define GLOBAL_NAME ::vl_workflow_global::ClassMethod::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::ClassMethod::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassMethod)
	vl_workflow_global::ClassMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Empty;
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
			auto a = ::vl::Ptr<::Methods>(new ::Methods());
			(GLOBAL_NAME s = (GLOBAL_NAME s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->x)) + ::vl::WString(L"]", false))));
		}
		{
			auto a = ::vl::Ptr<::Methods>(new ::Methods(100));
			(GLOBAL_NAME s = (GLOBAL_NAME s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->x)) + ::vl::WString(L"]", false))));
		}
		{
			auto a = ::vl::Ptr<::Methods>(new ::Methods(1));
			auto handler = ::vl::__vwsn::EventAttach(::vl::__vwsn::This(a.Obj())->XChanged, LAMBDA(::vl_workflow_global::__vwsnf1_ClassMethod_main_(a)));
			::vl::__vwsn::This(a.Obj())->SetX((::vl::__vwsn::This(a.Obj())->GetX() + 1));
			::vl::__vwsn::This(a.Obj())->SetX((::vl::__vwsn::This(a.Obj())->GetX() - 1));
			::vl::__vwsn::This(a.Obj())->SetX(::vl::__vwsn::This(a.Obj())->GetX());
			::vl::__vwsn::EventDetach(::vl::__vwsn::This(a.Obj())->XChanged, handler);
			::vl::__vwsn::This(a.Obj())->SetX(100);
			(GLOBAL_NAME s = (GLOBAL_NAME s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(a.Obj())->GetX())) + ::vl::WString(L"]", false))));
		}
		return GLOBAL_NAME s;
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
		(GLOBAL_NAME s = (GLOBAL_NAME s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(::vl::__vwsn::This(this)->a.Obj())->GetX())) + ::vl::WString(L"]", false))));
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


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
