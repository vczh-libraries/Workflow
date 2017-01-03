/***********************************************************************
Generated from ../Resources/Codegen/HelloWorld.txt
***********************************************************************/

#include "HelloWorld.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_HelloWorld)
	vl_workflow_global::HelloWorld instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_HelloWorld)

namespace vl_workflow_global
{
	::vl::WString HelloWorld::main()
	{
		throw 0;
	}

	HelloWorld& HelloWorld::Instance()
	{
		return Getvl_workflow_global_HelloWorld().instance;
	}
}

