/***********************************************************************
Generated from ../Resources/Codegen/Delete.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_DELETE
#define VCZH_WORKFLOW_CPP_GENERATED_DELETE

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
	class Delete
	{
	public:

		bool DoDelete(::test::ObservableValue* value);
		::vl::WString main();

		static Delete& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
