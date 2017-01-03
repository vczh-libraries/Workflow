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

		instance.s = L"";
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassCtor)

namespace vl_workflow_global
{
	::vl::WString ClassCtor::main()
	{
		throw 0;
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
	throw 0;
}

Base::Base(::vl::vint32_t _x)
{
	throw 0;
}

::vl::vint32_t Base::GetX()
{
	throw 0;
}

void Base::SetX(::vl::vint32_t _x)
{
	throw 0;
}

/***********************************************************************
Class (::Ctor)
***********************************************************************/

Ctor::Ctor()
{
	throw 0;
}

Ctor::Ctor(::vl::vint32_t y, bool b)
{
	throw 0;
}

Ctor::Ctor(bool b, ::vl::vint32_t value)
{
	throw 0;
}

Ctor::Ctor(::vl::vint32_t y, ::vl::vint32_t value)
{
	throw 0;
}

