/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE
#define VCZH_WORKFLOW_CPP_GENERATED_NEWCUSTOMINTERFACE

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class IMyInterface;

class IMyInterface : public virtual ::vl::reflection::description::IValueEnumerable, public ::vl::reflection::Description<IMyInterface>
{
public:

	static ::vl::vint32_t Get50();
	virtual ::vl::vint32_t Get100() = 0;
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class NewCustomInterface
	{
	public:

		::vl::WString main();

		static NewCustomInterface& Instance();
	};
}

#endif
