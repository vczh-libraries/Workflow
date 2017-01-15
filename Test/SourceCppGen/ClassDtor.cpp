/***********************************************************************
Generated from ../Resources/Codegen/ClassDtor.txt
***********************************************************************/

#include "ClassDtor.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassDtor)
	vl_workflow_global::ClassDtor instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassDtor)

namespace vl_workflow_global
{
	::vl::WString ClassDtor::main()
	{
		::Dtor* x = new ::Dtor();
		(::vl::__vwsn::This(x)->name = ::vl::WString(L"x", false));
		::Dtor* y = new ::Dtor();
		(::vl::__vwsn::This(y)->name = ::vl::WString(L"y", false));
		::Dtor* z = new ::Dtor();
		(::vl::__vwsn::This(z)->name = ::vl::WString(L"z", false));
		(::vl::__vwsn::This(x)->next = y);
		(::vl::__vwsn::This(y)->next = z);
		::vl::__vwsn::This(x)->Dispose(true);
		return ::vl_workflow_global::ClassDtor::Instance().s;
	}

	ClassDtor& ClassDtor::Instance()
	{
		return Getvl_workflow_global_ClassDtor().instance;
	}
}

/***********************************************************************
Class (::Dtor)
***********************************************************************/

Dtor::Dtor()
{
}

Dtor::~Dtor()
{
	(::vl_workflow_global::ClassDtor::Instance().s = ((((::vl::WString(L"", false) + ::vl_workflow_global::ClassDtor::Instance().s) + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(this)->name) + ::vl::WString(L"]", false)));
	if ((::vl::__vwsn::This(this)->next != nullptr))
	{
		::vl::__vwsn::This(::vl::__vwsn::This(this)->next)->Dispose(true);
	}
}

