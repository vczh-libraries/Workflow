/***********************************************************************
Generated from ../Resources/Codegen/TryCatch.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_TRYCATCH
#define VCZH_WORKFLOW_CPP_GENERATED_TRYCATCH

#include "../Source/CppTypes.h"


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class TryCatch
	{
	public:

		::vl::WString log;

		void Test1();
		void Test2();
		void Throw(const ::vl::WString& message);
		void Test3();
		void Test4();
		::vl::WString main();

		static TryCatch& Instance();
	};
}

#endif
