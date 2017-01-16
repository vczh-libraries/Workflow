/***********************************************************************
Generated from ../Resources/Codegen/StaticMethod.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_STATICMETHOD
#define VCZH_WORKFLOW_CPP_GENERATED_STATICMETHOD

#include "../Source/CppTypes.h"


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class StaticMethod
	{
	public:

		::vl::Ptr<::test::ObservableValue> Create2(::vl::vint32_t value, const ::vl::WString& name);
		::vl::WString main();

		static StaticMethod& Instance();
	};
}

#endif
