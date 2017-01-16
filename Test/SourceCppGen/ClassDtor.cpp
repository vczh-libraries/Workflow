/***********************************************************************
Generated from ../Resources/Codegen/ClassDtor.txt
***********************************************************************/

#include "ClassDtor.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::ClassDtor::
#define GLOBAL_NAME ::vl_workflow_global::ClassDtor::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::ClassDtor::Instance()

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
		auto x = new ::Dtor();
		(::vl::__vwsn::This(x)->name = ::vl::WString(L"x", false));
		auto y = new ::Dtor();
		(::vl::__vwsn::This(y)->name = ::vl::WString(L"y", false));
		auto z = new ::Dtor();
		(::vl::__vwsn::This(z)->name = ::vl::WString(L"z", false));
		(::vl::__vwsn::This(x)->next = y);
		(::vl::__vwsn::This(y)->next = z);
		::vl::__vwsn::This(x)->Dispose(true);
		return GLOBAL_NAME s;
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
	(GLOBAL_NAME s = ((((::vl::WString(L"", false) + GLOBAL_NAME s) + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(this)->name) + ::vl::WString(L"]", false)));
	if ((::vl::__vwsn::This(this)->next != nullptr))
	{
		::vl::__vwsn::This(::vl::__vwsn::This(this)->next)->Dispose(true);
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
