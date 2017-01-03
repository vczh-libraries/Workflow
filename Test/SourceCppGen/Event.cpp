/***********************************************************************
Generated from ../Resources/Codegen/Event.txt
***********************************************************************/

#include "Event.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Event)
	vl_workflow_global::Event instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.olds = ::vl::reflection::description::IValueList::Create();
		instance.news = ::vl::reflection::description::IValueList::Create();
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Event)

namespace vl_workflow_global
{
	void Event::EventHandler(::vl::vint32_t oldValue, ::vl::vint32_t newValue)
	{
		throw 0;
	}

	::vl::WString Event::main()
	{
		throw 0;
	}

	Event& Event::Instance()
	{
		return Getvl_workflow_global_Event().instance;
	}
}

