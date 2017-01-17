/***********************************************************************
Generated from ../Resources/Codegen/Event.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_EVENT
#define VCZH_WORKFLOW_CPP_GENERATED_EVENT

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif


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

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
