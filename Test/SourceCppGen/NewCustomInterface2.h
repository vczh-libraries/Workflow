/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface2.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE2
#define VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE2

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class IMyInterface;

class IMyInterface : public virtual ::vl::reflection::description::IValueEnumerable, public ::vl::reflection::Description<IMyInterface>
{
public:

	::vl::vint32_t Get100();
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class NewCustomInterface2
	{
	public:

		::vl::WString main();

		static NewCustomInterface2& Instance();
	};
}

#endif
