/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface3.txt
***********************************************************************/

#include "NewCustomInterface3.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface3)
	vl_workflow_global::NewCustomInterface3 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface3)

namespace vl_workflow_global
{
	::vl::WString NewCustomInterface3::main()
	{
		throw 0;
	}

	NewCustomInterface3& NewCustomInterface3::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface3().instance;
	}
}

/***********************************************************************
Lambda Classes (Declaration)
***********************************************************************/



/***********************************************************************
Lambda Classes (Implementation)
***********************************************************************/



/***********************************************************************
Class (::IMyInterface)
***********************************************************************/

::vl::vint32_t ::IMyInterface::Get100()
{
	throw 0;
}

/***********************************************************************
Class (::MyClass)
***********************************************************************/

::MyClass::MyClass(::vl::vint32_t _begin, ::vl::vint32_t _end)
{
	throw 0;
}

::vl::vint32_t ::MyClass::Get50()
{
	throw 0;
}

::vl::Ptr<::IMyInterface> ::MyClass::CreateMyInterface()
{
	throw 0;
}

