/***********************************************************************
Generated from ../Resources/Codegen/HelloWorld.txt
***********************************************************************/

#include "HelloWorld.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::HelloWorld::
#define GLOBAL_NAME ::vl_workflow_global::HelloWorld::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::HelloWorld::Instance()

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
		return ::vl::WString(L"Hello, world!", false);
	}

	HelloWorld& HelloWorld::Instance()
	{
		return Getvl_workflow_global_HelloWorld().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
