/***********************************************************************
Generated from ../Resources/Codegen/Event.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_EVENT
#define VCZH_WORKFLOW_CPP_GENERATED_EVENT

#include "../Source/CppTypes.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class Event
	{
	public:

		::vl::Ptr<::vl::reflection::description::IValueList> olds;
		::vl::Ptr<::vl::reflection::description::IValueList> news;

		void EventHandler(::vl::vint32_t oldValue, ::vl::vint32_t newValue);
		::vl::WString main();

		static Event& Instance();
	};
}

#endif
